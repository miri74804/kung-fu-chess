#include "GameServer.h"
#include "../src/model/Piece.h"
#include "../src/protocol/Protocol.h"
#include <chrono>
#include <iostream>
#include <thread>

namespace {
	constexpr int TICK_MS = 30;

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
		handleOpen(connectionState, webSocket);
	}
	else if (msg->type == ix::WebSocketMessageType::Close) {
		handleClose(connectionState);
	}
	else if (msg->type == ix::WebSocketMessageType::Message) {
		handleMove(connectionState, webSocket, msg->str);
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

void GameServer::handleOpen(const std::shared_ptr<ix::ConnectionState>& connectionState, ix::WebSocket& webSocket) {
	Color seat = assignSeat(connectionState->getId());
	std::cout << "Client connected - assigned " << seatName(seat) << "\n";
	webSocket.send(Protocol::encodeAssignment(seat));
}

void GameServer::handleClose(const std::shared_ptr<ix::ConnectionState>& connectionState) {
	std::cout << "Client disconnected\n";
	std::lock_guard<std::mutex> lock(seatsMutex);
	seatsByConnectionId.erase(connectionState->getId());
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
		std::this_thread::sleep_for(std::chrono::milliseconds(TICK_MS));

		auto now = std::chrono::steady_clock::now();
		int elapsedMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick).count());
		lastTick = now;

		{
			std::lock_guard<std::mutex> lock(engineMutex);
			engine.advanceTime(elapsedMs);
		}

		broadcastSnapshot();
	}
}
