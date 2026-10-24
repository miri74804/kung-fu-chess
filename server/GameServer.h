#pragma once

#include "Room.h"
#include "db/UserStore.h"
#include "network/NetworkServer.h"
#include "../core/model/Board.h"
#include <map>
#include <memory>
#include <mutex>
#include <string>

// Routes each connection to its Room. Owns a template Board (parsed once at
// startup, e.g. from stdin) that's deep-cloned into a fresh board every time
// a client asks to create a room - itself knows nothing about seats, the
// game engine, or auto-resign; all of that lives in Room now. Also owns the
// UserStore (accounts/ratings) and gates create_room/join_room behind a
// successful login - a connection that never logged in has no identity to
// hand a Room. Drives the fixed-interval tick loop across every
// currently-live room.
class GameServer {
public:
	GameServer(Board templateBoard, int port);

	// Blocks forever, ticking every room on a fixed interval.
	void run();

private:
	void onOpen(const std::string& connectionId, ix::WebSocket& webSocket);
	void onClose(const std::string& connectionId);
	void onMessage(const std::string& connectionId, ix::WebSocket& webSocket, const std::string& text);

	void handleLogin(const std::string& connectionId, ix::WebSocket& webSocket, const std::string& text);
	void handleCreateRoom(const std::string& connectionId, ix::WebSocket& webSocket, const std::string& text);
	void handleJoinRoom(const std::string& connectionId, ix::WebSocket& webSocket, const std::string& text);

	std::string generateRoomId();

	// Everything the server needs to remember about a connection once it's
	// logged in - looked up again for create_room/join_room, so neither of
	// those messages needs to (or can be trusted to) restate the username.
	struct AuthedUser {
		std::string username;
		int rating;
	};

	Board templateBoard;
	NetworkServer networkServer;
	UserStore userStore;

	mutable std::mutex authMutex;
	std::map<std::string, AuthedUser> authedUsers;

	mutable std::mutex roomsMutex;
	std::map<std::string, std::unique_ptr<Room>> rooms;
	std::map<std::string, std::string> roomIdByConnection;
};
