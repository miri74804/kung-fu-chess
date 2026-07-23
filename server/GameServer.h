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

// Owns the one GameEngine and the WebSocket server for this game. A seat
// (White, then Black, then read-only viewers) is decided once a connecting
// client's login message reveals its username - not at the raw socket
// Open, since that's the earliest point a returning player can be told
// apart from a brand-new one: if the username matches a seat that's
// currently mid-disconnect, that seat is handed back to them instead of
// making them a viewer. Also broadcasts a GameSnapshot to everyone every
// tick, and auto-resigns a seated player who never comes back within
// AUTO_RESIGN_MS.
class GameServer {
public:
	GameServer(Board& board, int port);

	// Blocks forever, broadcasting a snapshot on a fixed tick.
	void run();

private:
	void onClientMessage(const std::shared_ptr<ix::ConnectionState>& connectionState,
		ix::WebSocket& webSocket, const ix::WebSocketMessagePtr& msg);
	void handleClose(const std::shared_ptr<ix::ConnectionState>& connectionState);
	void handleMove(const std::shared_ptr<ix::ConnectionState>& connectionState, ix::WebSocket& webSocket, const std::string& text);
	void handleLogin(const std::shared_ptr<ix::ConnectionState>& connectionState, ix::WebSocket& webSocket, const std::string& text);
	void broadcastSnapshot();
	void broadcastPlayers();

	// First login becomes White, second Black, anyone after that is a
	// viewer (Color::NONE) - seats are never freed on disconnect by
	// themselves (see reclaimDisconnectedSeat for the one case where a
	// seat IS handed back).
	Color assignSeat(const std::string& connectionId);
	Color seatFor(const std::string& connectionId) const;

	// If a seat is currently mid-disconnect-countdown and its last-known
	// username matches, hands that seat back to this new connection and
	// cancels the countdown. Returns Color::NONE if there's nothing to
	// reclaim (the normal case) - callers fall back to assignSeat.
	Color reclaimDisconnectedSeat(const std::string& connectionId, const std::string& username);

	// Starts (or, if one's already running, leaves alone) the grace-period
	// countdown for a disconnected seat.
	void startResignCountdown(Color seat);

	// Called every tick: if the grace period just elapsed, resigns that
	// seat's game; otherwise broadcasts how much of it is left.
	void checkAutoResign();
	void broadcastDisconnectCountdown(Color color, int remainingMs);
	void broadcastDisconnectCleared();

	GameEngine engine;
	mutable std::mutex engineMutex;

	ix::WebSocketServer server;

	mutable std::mutex seatsMutex;
	std::map<std::string, Color> seatsByConnectionId;
	bool whiteTaken = false;
	bool blackTaken = false;

	// Just display labels - not tied to any account/password (that's a
	// separate, larger login feature) - but also doubles as "last known
	// occupant" for reclaimDisconnectedSeat. Defaults match what the
	// panels showed before this feature existed.
	mutable std::mutex namesMutex;
	std::string whiteName = "White";
	std::string blackName = "Black";

	mutable std::mutex disconnectMutex;
	bool resignCountdownActive = false;
	Color disconnectedSeat = Color::NONE;
	std::chrono::steady_clock::time_point resignDeadline;
};
