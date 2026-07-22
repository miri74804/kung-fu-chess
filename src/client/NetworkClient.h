#pragma once

#include "../engine/GameSnapshot.h"
#include "../model/Position.h"
#include <ixwebsocket/IXWebSocket.h>
#include <mutex>
#include <string>

// Owns the client's single WebSocket connection to the server. IXWebSocket
// delivers incoming messages on its own background thread, so the latest
// decoded snapshot is stored behind a mutex - Game's main loop (on the main
// thread) only ever reads a complete, consistent snapshot, never one being
// written mid-update.
class NetworkClient {
public:
	explicit NetworkClient(const std::string& url);
	~NetworkClient();

	void sendMove(const Position& source, const Position& destination);

	bool hasSnapshot() const;
	GameSnapshot latestSnapshot() const;

private:
	ix::WebSocket webSocket;
	mutable std::mutex snapshotMutex;
	GameSnapshot snapshot;
	bool received = false;
};
