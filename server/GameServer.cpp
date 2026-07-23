#include "GameServer.h"
#include "../src/Constants.h"
#include "../src/model/Piece.h"
#include "../src/protocol/Protocol.h"
#include <iostream>
#include <thread>

namespace {
	std::string seatName(Color seat) {
		if (seat == Color::White) return "White";
		if (seat == Color::Black) return "Black";
		return "viewer";
	}
}

GameServer::GameServer(Board& board, int port) : engine(board), server(port) {
	server.setOnClientMessageCallback(
		[this](std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket& webSocket, const ix::WebSocketMessagePtr& msg) {
			onClientMessage(connectionState, webSocket, msg);
		});
}

void GameServer::onClientMessage(const std::shared_ptr<ix::ConnectionState>& connectionState,
	ix::WebSocket& webSocket, const ix::WebSocketMessagePtr& msg) {
	if (msg->type == ix::WebSocketMessageType::Open) {
		std::cout << "Client connected\n";
	}
	else if (msg->type == ix::WebSocketMessageType::Close) {
		handleClose(connectionState);
	}
	else if (msg->type == ix::WebSocketMessageType::Message) {
		if (Protocol::peekType(msg->str) == "login") {
			handleLogin(connectionState, webSocket, msg->str);
		}
		else {
			handleMove(connectionState, webSocket, msg->str);
		}
	}
}

Color GameServer::assignSeat(const std::string& connectionId) {
	std::lock_guard<std::mutex> lock(seatsMutex);

	Color seat = Color::NONE;
	if (!whiteTaken) {
		seat = Color::White;
		whiteTaken = true;
	}
	else if (!blackTaken) {
		seat = Color::Black;
		blackTaken = true;
	}

	seatsByConnectionId[connectionId] = seat;
	return seat;
}

Color GameServer::seatFor(const std::string& connectionId) const {
	std::lock_guard<std::mutex> lock(seatsMutex);
	auto it = seatsByConnectionId.find(connectionId);
	return it != seatsByConnectionId.end() ? it->second : Color::NONE;
}

Color GameServer::reclaimDisconnectedSeat(const std::string& connectionId, const std::string& username) {
	std::lock_guard<std::mutex> lock(disconnectMutex);
	if (!resignCountdownActive) {
		return Color::NONE;
	}

	std::string expectedName;
	{
		std::lock_guard<std::mutex> namesLock(namesMutex);
		expectedName = disconnectedSeat == Color::White ? whiteName : blackName;
	}

	if (expectedName != username) {
		return Color::NONE;
	}

	Color reclaimed = disconnectedSeat;
	resignCountdownActive = false;

	std::lock_guard<std::mutex> seatsLock(seatsMutex);
	seatsByConnectionId[connectionId] = reclaimed;
	return reclaimed;
}

void GameServer::handleLogin(const std::shared_ptr<ix::ConnectionState>& connectionState,
	ix::WebSocket& webSocket, const std::string& text) {
	Protocol::Login login = Protocol::decodeLogin(text);
	if (!login.isValid) {
		return;
	}

	Color seat = reclaimDisconnectedSeat(connectionState->getId(), login.username);
	if (seat != Color::NONE) {
		std::cout << login.username << " reconnected as " << seatName(seat) << "\n";
		broadcastDisconnectCleared();
	}
	else {
		seat = assignSeat(connectionState->getId());
		std::cout << "Assigned " << seatName(seat) << " to " << login.username << "\n";

		if (seat != Color::NONE) {
			std::lock_guard<std::mutex> lock(namesMutex);
			if (seat == Color::White) {
				whiteName = login.username;
			}
			else {
				blackName = login.username;
			}
		}
	}

	webSocket.send(Protocol::encodeAssignment(seat));
	broadcastPlayers();
}

void GameServer::broadcastPlayers() {
	std::string message;
	{
		std::lock_guard<std::mutex> lock(namesMutex);
		message = Protocol::encodePlayers(whiteName, blackName);
	}

	for (const std::shared_ptr<ix::WebSocket>& client : server.getClients()) {
		client->send(message);
	}
}

void GameServer::handleClose(const std::shared_ptr<ix::ConnectionState>& connectionState) {
	std::cout << "Client disconnected\n";

	Color seat;
	{
		std::lock_guard<std::mutex> lock(seatsMutex);
		auto it = seatsByConnectionId.find(connectionState->getId());
		seat = it != seatsByConnectionId.end() ? it->second : Color::NONE;
		seatsByConnectionId.erase(connectionState->getId());
	}

	if (seat != Color::NONE) {
		startResignCountdown(seat);
	}
}

void GameServer::startResignCountdown(Color seat) {
	std::lock_guard<std::mutex> lock(disconnectMutex);
	if (resignCountdownActive) {
		return; // a countdown (for either seat) is already running
	}

	std::lock_guard<std::mutex> engineLock(engineMutex);
	if (engine.isGameOver()) {
		return; // nothing to resign into
	}

	resignCountdownActive = true;
	disconnectedSeat = seat;
	resignDeadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(AUTO_RESIGN_MS);
}

void GameServer::checkAutoResign() {
	bool shouldResign = false;
	Color resigningColor = Color::NONE;
	int remainingMs = 0;

	{
		std::lock_guard<std::mutex> lock(disconnectMutex);
		if (!resignCountdownActive) {
			return;
		}

		auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
			resignDeadline - std::chrono::steady_clock::now()).count();

		if (remaining <= 0) {
			shouldResign = true;
			resigningColor = disconnectedSeat;
			resignCountdownActive = false;
		}
		else {
			resigningColor = disconnectedSeat;
			remainingMs = static_cast<int>(remaining);
		}
	}

	if (shouldResign) {
		std::lock_guard<std::mutex> lock(engineMutex);
		engine.resign(resigningColor);
		std::cout << seatName(resigningColor) << " didn't reconnect in time - auto-resigned\n";
	}
	else {
		broadcastDisconnectCountdown(resigningColor, remainingMs);
	}
}

void GameServer::broadcastDisconnectCountdown(Color color, int remainingMs) {
	std::string message = Protocol::encodeDisconnectCountdown(color, remainingMs);
	for (const std::shared_ptr<ix::WebSocket>& client : server.getClients()) {
		client->send(message);
	}
}

void GameServer::broadcastDisconnectCleared() {
	std::string message = Protocol::encodeDisconnectCleared();
	for (const std::shared_ptr<ix::WebSocket>& client : server.getClients()) {
		client->send(message);
	}
}

void GameServer::handleMove(const std::shared_ptr<ix::ConnectionState>& connectionState,
	ix::WebSocket& webSocket, const std::string& text) {
	Protocol::MoveCommand command = Protocol::decodeMoveCommand(text);
	if (!command.isValid) {
		return;
	}

	Color seat = seatFor(connectionState->getId());
	if (seat == Color::NONE) {
		// Viewers can't move anything - still tell them so the client can
		// flash the same "rejected" feedback a player would get.
		webSocket.send(Protocol::encodeRejection(command.destination));
		return;
	}

	bool accepted;
	{
		std::lock_guard<std::mutex> lock(engineMutex);
		Piece* movingPiece = engine.getBoard().getPieceAt(command.source);
		if (movingPiece == nullptr || movingPiece->getColor() != seat) {
			accepted = false; // can only move your own pieces
		}
		else {
			accepted = engine.requestMove(command.source, command.destination).isAccepted;
		}
	}

	if (!accepted) {
		webSocket.send(Protocol::encodeRejection(command.destination));
	}
}

void GameServer::broadcastSnapshot() {
	std::string message;
	{
		std::lock_guard<std::mutex> lock(engineMutex);
		message = Protocol::encodeSnapshot(engine.snapshot());
	}

	for (const std::shared_ptr<ix::WebSocket>& client : server.getClients()) {
		client->send(message);
	}
}

void GameServer::run() {
	auto listenResult = server.listen();
	if (!listenResult.first) {
		std::cerr << listenResult.second << "\n";
		return;
	}

	server.start();
	std::cout << "Server listening" << std::endl;

	auto lastTick = std::chrono::steady_clock::now();
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_TICK_MS));

		auto now = std::chrono::steady_clock::now();
		int elapsedMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick).count());
		lastTick = now;

		{
			std::lock_guard<std::mutex> lock(engineMutex);
			engine.advanceTime(elapsedMs);
		}

		checkAutoResign();
		broadcastSnapshot();
	}
}
