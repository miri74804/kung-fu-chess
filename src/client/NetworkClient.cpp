#include "NetworkClient.h"
#include "../protocol/Protocol.h"
#include <iostream>

NetworkClient::NetworkClient(const std::string& url) {
	webSocket.setUrl(url);
	webSocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
		if (msg->type == ix::WebSocketMessageType::Message) {
			handleMessage(msg->str);
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

void NetworkClient::handleMessage(const std::string& text) {
	std::string type = Protocol::peekType(text);

	if (type == "snapshot") {
		GameSnapshot decoded = Protocol::decodeSnapshot(text);
		std::lock_guard<std::mutex> lock(snapshotMutex);
		snapshot = decoded;
		received = true;
	}
	else if (type == "assigned") {
		Protocol::Assignment assignment = Protocol::decodeAssignment(text);
		if (assignment.isValid) {
			{
				std::lock_guard<std::mutex> lock(colorMutex);
				myColor = assignment.color;
			}
			if (assignment.color == Color::White) {
				std::cout << "You are playing: White\n";
			}
			else if (assignment.color == Color::Black) {
				std::cout << "You are playing: Black\n";
			}
			else {
				std::cout << "Both seats are taken - you're a viewer (can watch, can't move pieces)\n";
			}
		}
	}
	else if (type == "reject") {
		Protocol::Rejection rejection = Protocol::decodeRejection(text);
		if (rejection.isValid) {
			std::lock_guard<std::mutex> lock(rejectionMutex);
			rejectionPending = true;
			rejectionPosition = rejection.position;
		}
	}
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

Color NetworkClient::assignedColor() const {
	std::lock_guard<std::mutex> lock(colorMutex);
	return myColor;
}

bool NetworkClient::consumeRejection(Position& outPosition) {
	std::lock_guard<std::mutex> lock(rejectionMutex);
	if (rejectionPending) {
		outPosition = rejectionPosition;
		rejectionPending = false;
		return true;
	}
	return false;
}
