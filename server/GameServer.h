#pragma once

#include "network/NetworkServer.h"
#include "../core/engine/GameEngine.h"
#include "../core/model/Board.h"
#include "../core/model/Color.h"
#include <chrono>
#include <map>
#include <mutex>
#include <string>

// Owns the one GameEngine and reacts to NetworkServer's connection events -
// itself knows nothing about ix::WebSocketServer/ix::ConnectionState, only
// connection ids and a WebSocket reference for direct replies. A seat
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
	void onOpen(const std::string& connectionId, ix::WebSocket& webSocket);
	void onClose(const std::string& connectionId);
	void onMessage(const std::string& connectionId, ix::WebSocket& webSocket, const std::string& text);

	void handleLogin(const std::string& connectionId, ix::WebSocket& webSocket, const std::string& text);
	void handleMove(const std::string& connectionId, ix::WebSocket& webSocket, const std::string& text);
	void broadcastSnapshot();
	void broadcastPlayers();

	// First login becomes White, second Black, anyone after that is a
	// viewer (Color::NONE) - seats are never freed on disconnect by
	// themselves (see reclaimDisconnectedSeat for the one case where a
	// seat IS handed back).
	Color assignSeat(const std::string& connectionId);
	Color seatFor(const std::string& connectionId) const;

	// If either seat is currently mid-disconnect-countdown and its
	// last-known username matches, hands that seat back to this new
	// connection and cancels its countdown. Returns Color::NONE if there's
	// nothing to reclaim (the normal case) - callers fall back to assignSeat.
	Color reclaimDisconnectedSeat(const std::string& connectionId, const std::string& username);

	// Starts (or, if one's already running for this seat, leaves alone)
	// the grace-period countdown for a disconnected seat. White and Black
	// are tracked independently, so both can be mid-disconnect at once.
	void startResignCountdown(Color seat);

	// Called every tick, once per seat: if that seat's grace period just
	// elapsed, resigns its game (unless the other seat's check already
	// ended it this same tick); otherwise broadcasts how much is left.
	void checkAutoResign();
	void checkSeatResign(Color seat);
	void broadcastDisconnectCountdown(Color color, int remainingMs);
	void broadcastDisconnectCleared();

	// Per-seat grace-period state after a disconnect - White and Black
	// each get their own, so one player's countdown is never lost or
	// overwritten just because the other also disconnected.
	struct DisconnectState {
		bool active = false;
		std::chrono::steady_clock::time_point deadline;
	};

	GameEngine engine;
	mutable std::mutex engineMutex;

	NetworkServer networkServer;

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
	DisconnectState whiteDisconnect;
	DisconnectState blackDisconnect;
};
