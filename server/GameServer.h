#pragma once

#include "../src/engine/GameEngine.h"
#include "../src/model/Board.h"
#include "../src/model/Color.h"
#include <ixwebsocket/IXConnectionState.h>
#include <ixwebsocket/IXWebSocketServer.h>
#include <map>
#include <memory>
#include <mutex>
#include <string>

// Owns the one GameEngine and the WebSocket server for this game: assigns
// each connecting client a seat (White, then Black, then read-only
// viewers - by connection order), enforces that a client can only move its
// own seat's pieces, and broadcasts a GameSnapshot to everyone every tick.
class GameServer {
public:
	GameServer(Board& board, int port);

	// Blocks forever, broadcasting a snapshot on a fixed tick.
	void run();

private:
	void onClientMessage(const std::shared_ptr<ix::ConnectionState>& connectionState,
		ix::WebSocket& webSocket, const ix::WebSocketMessagePtr& msg);
	void handleOpen(const std::shared_ptr<ix::ConnectionState>& connectionState, ix::WebSocket& webSocket);
	void handleClose(const std::shared_ptr<ix::ConnectionState>& connectionState);
	void handleMove(const std::shared_ptr<ix::ConnectionState>& connectionState, const std::string& text);
	void broadcastSnapshot();

	// First connection becomes White, second Black, anyone after that is a
	// viewer (Color::NONE) - seats are never freed on disconnect (auto-
	// resign/reconnect handling is a separate, not-yet-built feature).
	Color assignSeat(const std::string& connectionId);
	Color seatFor(const std::string& connectionId) const;

	GameEngine engine;
	mutable std::mutex engineMutex;

	ix::WebSocketServer server;

	mutable std::mutex seatsMutex;
	std::map<std::string, Color> seatsByConnectionId;
	bool whiteTaken = false;
	bool blackTaken = false;
};
