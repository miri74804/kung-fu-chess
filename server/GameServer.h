#pragma once

#include "Room.h"
#include "network/NetworkServer.h"
#include "../core/model/Board.h"
#include <map>
#include <memory>
#include <mutex>
#include <string>

// Routes each connection to its Room. Owns a template Board (parsed once at
// startup, e.g. from stdin) that's deep-cloned into a fresh board every time
// a client asks to create a room - itself knows nothing about seats, the
// game engine, or auto-resign; all of that lives in Room now. Also drives
// the fixed-interval tick loop across every currently-live room.
class GameServer {
public:
	GameServer(Board templateBoard, int port);

	// Blocks forever, ticking every room on a fixed interval.
	void run();

private:
	void onOpen(const std::string& connectionId, ix::WebSocket& webSocket);
	void onClose(const std::string& connectionId);
	void onMessage(const std::string& connectionId, ix::WebSocket& webSocket, const std::string& text);

	void handleCreateRoom(const std::string& connectionId, ix::WebSocket& webSocket, const std::string& text);
	void handleJoinRoom(const std::string& connectionId, ix::WebSocket& webSocket, const std::string& text);

	// Caller must already hold roomsMutex. Retries on the (very unlikely)
	// collision with an already-live room id.
	std::string generateRoomId();

	Board templateBoard;
	NetworkServer networkServer;

	mutable std::mutex roomsMutex;
	std::map<std::string, std::unique_ptr<Room>> rooms;
	std::map<std::string, std::string> roomIdByConnection;
};
