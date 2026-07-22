#include "NetworkClient.h"
#include "../protocol/Protocol.h"
#include <iostream>

NetworkClient::NetworkClient(const std::string& url) {
	webSocket.setUrl(url);
	webSocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
		if (msg->type == ix::WebSocketMessageType::Message) {
			GameSnapshot decoded = Protocol::decodeSnapshot(msg->str);
			std::lock_guard<std::mutex> lock(snapshotMutex);
			snapshot = decoded;
			received = true;
		}
		else if (msg->type == ix::WebSocketMessageType::Error) {
			// Printed so a connection failure (wrong address, server not
			// running, firewall blocking the port) is visible instead of
			// the client just silently waiting forever for a snapshot.
			std::cerr << "WebSocket error: " << msg->errorInfo.reason
				<< " (retries: " << msg->errorInfo.retries << ")\n";
		}
		else if (msg->type == ix::WebSocketMessageType::Open) {
			std::cerr << "WebSocket connected to server\n";
		}
		else if (msg->type == ix::WebSocketMessageType::Close) {
			std::cerr << "WebSocket connection closed\n";
		}
	});
	webSocket.start();
}

NetworkClient::~NetworkClient() {
	webSocket.stop();
}

void NetworkClient::sendMove(const Position& source, const Position& destination) {
	webSocket.send(Protocol::encodeMoveCommand(source, destination));
}

bool NetworkClient::hasSnapshot() const {
	std::lock_guard<std::mutex> lock(snapshotMutex);
	return received;
}

GameSnapshot NetworkClient::latestSnapshot() const {
	std::lock_guard<std::mutex> lock(snapshotMutex);
	return snapshot;
}
