#include "NetworkServer.h"
#include <iostream>

NetworkServer::NetworkServer(int port) : server(port) {
	server.setOnClientMessageCallback(
		[this](std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket& webSocket, const ix::WebSocketMessagePtr& msg) {
			onClientMessage(connectionState, webSocket, msg);
		});
}

void NetworkServer::onClientMessage(const std::shared_ptr<ix::ConnectionState>& connectionState,
	ix::WebSocket& webSocket, const ix::WebSocketMessagePtr& msg) {
	const std::string& connectionId = connectionState->getId();

	if (msg->type == ix::WebSocketMessageType::Open) {
		clientsById[connectionId] = &webSocket;
		if (onOpenHandler) {
			onOpenHandler(connectionId, webSocket);
		}
	}
	else if (msg->type == ix::WebSocketMessageType::Close) {
		clientsById.erase(connectionId);
		if (onCloseHandler) {
			onCloseHandler(connectionId);
		}
	}
	else if (msg->type == ix::WebSocketMessageType::Message) {
		if (onMessageHandler) {
			onMessageHandler(connectionId, webSocket, msg->str);
		}
	}
}

void NetworkServer::setOnOpen(OpenHandler handler) {
	onOpenHandler = std::move(handler);
}

void NetworkServer::setOnClose(CloseHandler handler) {
	onCloseHandler = std::move(handler);
}

void NetworkServer::setOnMessage(MessageHandler handler) {
	onMessageHandler = std::move(handler);
}

bool NetworkServer::start() {
	auto listenResult = server.listen();
	if (!listenResult.first) {
		std::cerr << listenResult.second << "\n";
		return false;
	}

	server.start();
	std::cout << "Server listening" << std::endl;
	return true;
}

void NetworkServer::broadcast(const std::string& message) {
	for (const std::shared_ptr<ix::WebSocket>& client : server.getClients()) {
		client->send(message);
	}
}

void NetworkServer::sendTo(const std::string& connectionId, const std::string& message) {
	auto it = clientsById.find(connectionId);
	if (it != clientsById.end()) {
		it->second->send(message);
	}
}

void NetworkServer::broadcastTo(const std::set<std::string>& connectionIds, const std::string& message) {
	for (const std::string& connectionId : connectionIds) {
		sendTo(connectionId, message);
	}
}
