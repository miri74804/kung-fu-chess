#pragma once

#include "../engine/GameSnapshot.h"
#include "../model/Color.h"
#include "../model/Position.h"
#include <ixwebsocket/IXWebSocket.h>
#include <mutex>
#include <string>

// Owns the client's single WebSocket connection to the server. IXWebSocket
// delivers incoming messages on its own background thread, so every piece
// of state below is stored behind a mutex - Game's main loop (on the main
// thread) only ever reads complete, consistent values, never one being
// written mid-update.
class NetworkClient {
public:
	// username is sent to the server as soon as the connection opens (a
	// display label only - no password/account behind it yet).
	NetworkClient(const std::string& url, const std::string& username);
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

	// True (with color/remainingMs filled) while the server is actively
	// broadcasting a disconnect countdown - unlike consumeRejection, this
	// isn't edge-triggered: it just reflects the latest value received, since
	// the server keeps re-sending it every tick until the grace period ends.
	struct DisconnectStatus {
		bool active;
		Color color;
		int remainingMs;
	};
	DisconnectStatus disconnectStatus() const;

	// The two seats' display names, as last broadcast by the server -
	// "White"/"Black" until a login message updates them.
	struct PlayerNames {
		std::string whiteName;
		std::string blackName;
	};
	PlayerNames playerNames() const;

private:
	void handleMessage(const std::string& text);

	ix::WebSocket webSocket;
	std::string username;

	mutable std::mutex snapshotMutex;
	GameSnapshot snapshot;
	bool received = false;

	mutable std::mutex colorMutex;
	Color myColor = Color::NONE;

	mutable std::mutex rejectionMutex;
	bool rejectionPending = false;
	Position rejectionPosition;

	mutable std::mutex disconnectMutex;
	DisconnectStatus lastDisconnectStatus{ false, Color::NONE, 0 };

	mutable std::mutex namesMutex;
	PlayerNames lastPlayerNames{ "White", "Black" };
};
