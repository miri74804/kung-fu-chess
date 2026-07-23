#pragma once

#include "../engine/GameSnapshot.h"
#include "../model/Color.h"
#include "../model/Position.h"
#include <ixwebsocket/IXWebSocket.h>
#include <mutex>
#include <string>

// Owns the client's single WebSocket connection to the server. IXWebSocket
// delivers incoming messages on its own background thread, so both the
// latest snapshot and the assigned color are stored behind a mutex - Game's
// main loop (on the main thread) only ever reads complete, consistent
// values, never one being written mid-update.
class NetworkClient {
public:
	explicit NetworkClient(const std::string& url);
	~NetworkClient();

	void sendMove(const Position& source, const Position& destination);

	bool hasSnapshot() const;
	GameSnapshot latestSnapshot() const;

	// Color::NONE until the server's "assigned" message arrives, then
	// White/Black for a player or NONE again if this connection is a
	// viewer (both seats were already taken).
	Color assignedColor() const;

	// Returns true and fills outPosition exactly once per rejection
	// received (edge-triggered, like RealTimeArbiter::consumeCompletedMove) -
	// a second call with no new rejection in between returns false.
	bool consumeRejection(Position& outPosition);

private:
	void handleMessage(const std::string& text);

	ix::WebSocket webSocket;

	mutable std::mutex snapshotMutex;
	GameSnapshot snapshot;
	bool received = false;

	mutable std::mutex colorMutex;
	Color myColor = Color::NONE;

	mutable std::mutex rejectionMutex;
	bool rejectionPending = false;
	Position rejectionPosition;
};
