#pragma once

#include "../src/engine/GameEngine.h"
#include "../src/model/Board.h"
#include "../src/model/Color.h"
#include <chrono>
#include <ixwebsocket/IXConnectionState.h>
#include <ixwebsocket/IXWebSocketServer.h>
#include <map>
#include <memory>
#include <mutex>
#include <string>

// Owns the one GameEngine and the WebSocket server for this game: assigns
// each connecting client a seat (White, then Black, then read-only
// viewers - by connection order), enforces that a client can only move its
// own seat's pieces, broadcasts a GameSnapshot to everyone every tick, and
// auto-resigns a seated player who disconnects and doesn't come back
// within AUTO_RESIGN_MS.
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
	void handleMove(const std::shared_ptr<ix::ConnectionState>& connectionState, ix::WebSocket& webSocket, const std::string& text);
	void broadcastSnapshot();

	// First connection becomes White, second Black, anyone after that is a
	// viewer (Color::NONE) - seats are never freed on disconnect (a
	// reconnecting client is indistinguishable from a brand-new one, so it
	// just becomes a viewer too - there's no session token to reunite it
	// with its old seat).
	Color assignSeat(const std::string& connectionId);
	Color seatFor(const std::string& connectionId) const;

	// Starts (or, if one's already running, leaves alone) the grace-period
	// countdown for a disconnected seat.
	void startResignCountdown(Color seat);

	// Called every tick: if the grace period just elapsed, resigns that
	// seat's game; otherwise broadcasts how much of it is left.
	void checkAutoResign();
	void broadcastDisconnectCountdown(Color color, int remainingMs);

	GameEngine engine;
	mutable std::mutex engineMutex;

	ix::WebSocketServer server;

	mutable std::mutex seatsMutex;
	std::map<std::string, Color> seatsByConnectionId;
	bool whiteTaken = false;
	bool blackTaken = false;

	mutable std::mutex disconnectMutex;
	bool resignCountdownActive = false;
	Color disconnectedSeat = Color::NONE;
	std::chrono::steady_clock::time_point resignDeadline;
};
