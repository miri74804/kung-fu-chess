#include "NetworkClient.h"
#include "../../core/protocol/Protocol.h"
#include <exception>
#include <iostream>

NetworkClient::NetworkClient(const std::string& url, const std::string& username) : username(username) {
	webSocket.setUrl(url);
	webSocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
		onMessage(msg);
	});
	webSocket.start();
}

NetworkClient::~NetworkClient() {
	webSocket.stop();
}

void NetworkClient::onMessage(const ix::WebSocketMessagePtr& msg) {
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
		webSocket.send(Protocol::encodeLogin(username));
	}
	else if (msg->type == ix::WebSocketMessageType::Close) {
		std::cerr << "WebSocket connection closed\n";
	}
}

void NetworkClient::handleMessage(const std::string& text) {
	std::string type = Protocol::peekType(text);

	if (type == "snapshot") {
		handleSnapshotMessage(text);
	}
	else if (type == "assigned") {
		handleAssignedMessage(text);
	}
	else if (type == "reject") {
		handleRejectMessage(text);
	}
	else if (type == "disconnect_countdown") {
		handleDisconnectCountdownMessage(text);
	}
	else if (type == "disconnect_cleared") {
		handleDisconnectClearedMessage();
	}
	else if (type == "players") {
		handlePlayersMessage(text);
	}
}

void NetworkClient::handleSnapshotMessage(const std::string& text) {
	try {
		GameSnapshot decoded = Protocol::decodeSnapshot(text);
		std::lock_guard<std::mutex> lock(snapshotMutex);
		snapshot = decoded;
		received = true;
	}
	catch (const std::exception&) {
		// A malformed snapshot (e.g. torn mid-broadcast) - keep the last
		// good one instead of crashing this message-handling callback
		// (unlike the other decode* functions, decodeSnapshot has no
		// isValid fallback to check first).
	}
}

void NetworkClient::handleAssignedMessage(const std::string& text) {
	Protocol::Assignment assignment = Protocol::decodeAssignment(text);
	if (!assignment.isValid) {
		return;
	}

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

void NetworkClient::handleRejectMessage(const std::string& text) {
	Protocol::Rejection rejection = Protocol::decodeRejection(text);
	if (rejection.isValid) {
		std::lock_guard<std::mutex> lock(rejectionMutex);
		rejectionPending = true;
		rejectionPosition = rejection.position;
	}
}

void NetworkClient::handleDisconnectCountdownMessage(const std::string& text) {
	Protocol::DisconnectCountdown countdown = Protocol::decodeDisconnectCountdown(text);
	if (countdown.isValid) {
		std::lock_guard<std::mutex> lock(disconnectMutex);
		lastDisconnectStatus = { true, countdown.color, countdown.remainingMs };
	}
}

void NetworkClient::handleDisconnectClearedMessage() {
	std::lock_guard<std::mutex> lock(disconnectMutex);
	lastDisconnectStatus = { false, Color::NONE, 0 };
}

void NetworkClient::handlePlayersMessage(const std::string& text) {
	Protocol::Players players = Protocol::decodePlayers(text);
	if (players.isValid) {
		std::lock_guard<std::mutex> lock(namesMutex);
		lastPlayerNames = { players.whiteName, players.blackName };
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

NetworkClient::DisconnectStatus NetworkClient::disconnectStatus() const {
	std::lock_guard<std::mutex> lock(disconnectMutex);
	return lastDisconnectStatus;
}

NetworkClient::PlayerNames NetworkClient::playerNames() const {
	std::lock_guard<std::mutex> lock(namesMutex);
	return lastPlayerNames;
}
