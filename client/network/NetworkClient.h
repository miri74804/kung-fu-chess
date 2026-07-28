#pragma once

#include "../../core/engine/GameSnapshot.h"
#include "../../core/model/Color.h"
#include "../../core/model/Position.h"
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
	// username, and whether to create a brand-new room or join roomId, are
	// sent to the server as soon as the connection opens (username is a
	// display label only - no password/account behind it yet). roomId is
	// ignored when isCreate is true.
	NetworkClient(const std::string& url, const std::string& username, bool isCreate, const std::string& roomId);
	~NetworkClient();

	void sendMove(const Position& source, const Position& destination);

	bool hasSnapshot() const;
	GameSnapshot latestSnapshot() const;

	// Color::NONE until the server's "room_joined" message arrives, then
	// White/Black for a player or NONE again if this connection is a
	// viewer (both seats were already taken).
	Color assignedColor() const;

	// Empty until "room_joined" arrives, then the room's id - for Create,
	// this is the only place the newly-generated id is ever revealed.
	std::string roomId() const;

	// Set once if the server replied with "room_error" (e.g. joining an
	// unknown room id) instead of "room_joined" - empty otherwise.
	std::string roomError() const;

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
	// The WebSocket callback (set in the constructor) is a thin forwarder
	// to this - same convention as GameServer::onClientMessage - so the
	// actual per-event-type logic lives in a named, testable method
	// instead of an ever-growing lambda body.
	void onMessage(const ix::WebSocketMessagePtr& msg);

	// Dispatches by Protocol::peekType - one handler per message type,
	// each responsible for decoding and updating its own piece of state.
	void handleMessage(const std::string& text);
	void handleSnapshotMessage(const std::string& text);
	void handleRoomJoinedMessage(const std::string& text);
	void handleRoomErrorMessage(const std::string& text);
	void handleRejectMessage(const std::string& text);
	void handleDisconnectCountdownMessage(const std::string& text);
	void handleDisconnectClearedMessage();
	void handlePlayersMessage(const std::string& text);

	ix::WebSocket webSocket;
	std::string username;
	bool isCreate;
	std::string joinRoomId;

	mutable std::mutex snapshotMutex;
	GameSnapshot snapshot;
	bool received = false;

	mutable std::mutex colorMutex;
	Color myColor = Color::NONE;

	mutable std::mutex roomMutex;
	std::string currentRoomId;

	mutable std::mutex roomErrorMutex;
	std::string lastRoomError;

	mutable std::mutex rejectionMutex;
	bool rejectionPending = false;
	Position rejectionPosition;

	mutable std::mutex disconnectMutex;
	DisconnectStatus lastDisconnectStatus{ false, Color::NONE, 0 };

	mutable std::mutex namesMutex;
	PlayerNames lastPlayerNames{ "White", "Black" };
};
