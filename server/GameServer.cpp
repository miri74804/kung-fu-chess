#include "GameServer.h"
#include "../core/Constants.h"
#include "../core/protocol/Protocol.h"
#include <cctype>
#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

namespace {
	std::string randomRoomId() {
		// Excludes characters that are easy to misread off a small on-screen
		// banner and re-type wrong: 0/O, 1/I/L.
		static const char alphabet[] = "ABCDEFGHJKMNPQRSTUVWXYZ23456789";
		static std::mt19937 rng(std::random_device{}());
		std::uniform_int_distribution<int> dist(0, static_cast<int>(sizeof(alphabet)) - 2);

		std::string id;
		for (int i = 0; i < 6; ++i) {
			id += alphabet[dist(rng)];
		}
		return id;
	}

	// Defense in depth alongside the client's ES_UPPERCASE edit box: room
	// ids are always generated uppercase, so normalize whatever a client
	// sends before comparing it against the rooms map.
	std::string normalizeRoomId(const std::string& roomId) {
		std::string normalized;
		normalized.reserve(roomId.size());
		for (char c : roomId) {
			if (!std::isspace(static_cast<unsigned char>(c))) {
				normalized += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
			}
		}
		return normalized;
	}
}

GameServer::GameServer(Board templateBoard, int port) : templateBoard(std::move(templateBoard)), networkServer(port) {
	networkServer.setOnOpen([this](const std::string& connectionId, ix::WebSocket& webSocket) {
		onOpen(connectionId, webSocket);
	});
	networkServer.setOnClose([this](const std::string& connectionId) {
		onClose(connectionId);
	});
	networkServer.setOnMessage([this](const std::string& connectionId, ix::WebSocket& webSocket, const std::string& text) {
		onMessage(connectionId, webSocket, text);
	});
}

void GameServer::onOpen(const std::string&, ix::WebSocket&) {
	std::cout << "Client connected\n";
}

void GameServer::onClose(const std::string& connectionId) {
	std::cout << "Client disconnected\n";

	Room* room = nullptr;
	{
		std::lock_guard<std::mutex> lock(roomsMutex);
		auto it = roomIdByConnection.find(connectionId);
		if (it == roomIdByConnection.end()) {
			return;
		}

		auto roomIt = rooms.find(it->second);
		if (roomIt != rooms.end()) {
			room = roomIt->second.get();
		}
		roomIdByConnection.erase(it);
	}

	if (room != nullptr) {
		room->onClose(connectionId);
	}
}

void GameServer::onMessage(const std::string& connectionId, ix::WebSocket& webSocket, const std::string& text) {
	std::string type = Protocol::peekType(text);
	if (type == "create_room") {
		handleCreateRoom(connectionId, webSocket, text);
		return;
	}
	if (type == "join_room") {
		handleJoinRoom(connectionId, webSocket, text);
		return;
	}

	// Anything else (i.e. a move) belongs to whichever room this connection
	// already joined - a connection that hasn't created/joined a room yet
	// has nothing to route to, and is silently ignored.
	Room* room = nullptr;
	{
		std::lock_guard<std::mutex> lock(roomsMutex);
		auto it = roomIdByConnection.find(connectionId);
		if (it != roomIdByConnection.end()) {
			auto roomIt = rooms.find(it->second);
			if (roomIt != rooms.end()) {
				room = roomIt->second.get();
			}
		}
	}

	if (room != nullptr) {
		room->handleMove(connectionId, webSocket, text);
	}
}

std::string GameServer::generateRoomId() {
	std::string id;
	do {
		id = randomRoomId();
	} while (rooms.find(id) != rooms.end());
	return id;
}

void GameServer::handleCreateRoom(const std::string& connectionId, ix::WebSocket& webSocket, const std::string& text) {
	Protocol::CreateRoom command = Protocol::decodeCreateRoom(text);
	if (!command.isValid) {
		return;
	}

	Room* room;
	std::string roomId;
	{
		std::lock_guard<std::mutex> lock(roomsMutex);
		roomId = generateRoomId();
		auto inserted = rooms.emplace(roomId, std::make_unique<Room>(roomId, templateBoard.clone(), networkServer));
		room = inserted.first->second.get();
		roomIdByConnection[connectionId] = roomId;
	}

	std::cout << "Created room " << roomId << "\n";
	room->join(connectionId, webSocket, command.username);
}

void GameServer::handleJoinRoom(const std::string& connectionId, ix::WebSocket& webSocket, const std::string& text) {
	Protocol::JoinRoom command = Protocol::decodeJoinRoom(text);
	if (!command.isValid) {
		return;
	}

	std::string roomId = normalizeRoomId(command.roomId);

	Room* room = nullptr;
	{
		std::lock_guard<std::mutex> lock(roomsMutex);
		auto it = rooms.find(roomId);
		if (it != rooms.end()) {
			room = it->second.get();
			roomIdByConnection[connectionId] = roomId;
		}
	}

	if (room == nullptr) {
		webSocket.send(Protocol::encodeRoomError("not_found"));
		return;
	}

	room->join(connectionId, webSocket, command.username);
}

void GameServer::run() {
	if (!networkServer.start()) {
		return;
	}

	auto lastTick = std::chrono::steady_clock::now();
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_TICK_MS));

		auto now = std::chrono::steady_clock::now();
		int elapsedMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick).count());
		lastTick = now;

		std::vector<Room*> roomsSnapshot;
		{
			std::lock_guard<std::mutex> lock(roomsMutex);
			roomsSnapshot.reserve(rooms.size());
			for (auto& entry : rooms) {
				roomsSnapshot.push_back(entry.second.get());
			}
		}

		for (Room* room : roomsSnapshot) {
			room->tick(elapsedMs);
		}

		{
			std::lock_guard<std::mutex> lock(roomsMutex);
			for (auto it = rooms.begin(); it != rooms.end(); ) {
				if (it->second->isEmpty()) {
					it = rooms.erase(it);
				}
				else {
					++it;
				}
			}
		}
	}
}
