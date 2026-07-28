#pragma once

#include "network/NetworkServer.h"
#include "../core/engine/GameEngine.h"
#include "../core/model/Board.h"
#include "../core/model/Color.h"
#include <chrono>
#include <map>
#include <mutex>
#include <set>
#include <string>

// One self-contained game: owns its own Board+GameEngine, seats, display
// names, and disconnect timers - everything that used to be GameServer's
// single global set of members before rooms existed. GameServer now owns
// many of these and routes each connection's messages to the right one.
// Broadcasts (snapshot, players, disconnect countdowns) only ever reach
// connectionIds - this room's own participants - never the whole process.
class Room {
public:
	Room(std::string roomId, Board board, NetworkServer& networkServer);

	const std::string& getId() const { return roomId; }

	// First joiner becomes White, second Black, anyone after that is a
	// viewer (Color::NONE) - unless username matches a seat that's
	// currently mid-disconnect-countdown, in which case that seat is
	// handed back instead. Sends room_joined to the connection and
	// broadcasts the updated players message to the room.
	Color join(const std::string& connectionId, ix::WebSocket& webSocket, const std::string& username);

	void handleMove(const std::string& connectionId, ix::WebSocket& webSocket, const std::string& text);
	void onClose(const std::string& connectionId);

	// Advances the engine, resigns any seat whose disconnect grace period
	// just elapsed, and broadcasts a fresh snapshot - all scoped to this
	// room's own connections.
	void tick(int elapsedMs);

	// True once every connection that ever joined has left - GameServer
	// uses this to garbage-collect finished rooms.
	bool isEmpty() const;

private:
	Color assignSeat(const std::string& connectionId);
	Color seatFor(const std::string& connectionId) const;
	Color reclaimDisconnectedSeat(const std::string& connectionId, const std::string& username);

	void startResignCountdown(Color seat);
	void checkAutoResign();
	void checkSeatResign(Color seat);

	void broadcastSnapshot();
	void broadcastPlayers();
	void broadcastDisconnectCountdown(Color color, int remainingMs);
	void broadcastDisconnectCleared();
	void broadcastToRoom(const std::string& message);

	struct DisconnectState {
		bool active = false;
		std::chrono::steady_clock::time_point deadline;
	};

	std::string roomId;

	Board board;
	GameEngine engine;
	mutable std::mutex engineMutex;

	NetworkServer& networkServer;

	mutable std::mutex connectionsMutex;
	std::set<std::string> connectionIds;

	mutable std::mutex seatsMutex;
	std::map<std::string, Color> seatsByConnectionId;
	bool whiteTaken = false;
	bool blackTaken = false;

	mutable std::mutex namesMutex;
	std::string whiteName = "White";
	std::string blackName = "Black";

	mutable std::mutex disconnectMutex;
	DisconnectState whiteDisconnect;
	DisconnectState blackDisconnect;
};
