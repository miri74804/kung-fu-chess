#pragma once

#include <ixwebsocket/IXConnectionState.h>
#include <ixwebsocket/IXWebSocketServer.h>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>

// Thin wrapper around ix::WebSocketServer: owns listening/starting/
// broadcasting, and translates its connection-state-based callback into
// three simple, connection-id-based handlers - so a caller (GameServer)
// never needs to touch ix::ConnectionState or know how IXWebSocket shapes
// its events. Knows nothing about seats, the game engine, or the wire
// protocol - purely transport, so it could equally serve any future
// game session (e.g. one per room) without changes.
class NetworkServer {
public:
	explicit NetworkServer(int port);

	using OpenHandler = std::function<void(const std::string& connectionId, ix::WebSocket& webSocket)>;
	using CloseHandler = std::function<void(const std::string& connectionId)>;
	using MessageHandler = std::function<void(const std::string& connectionId, ix::WebSocket& webSocket, const std::string& text)>;

	void setOnOpen(OpenHandler handler);
	void setOnClose(CloseHandler handler);
	void setOnMessage(MessageHandler handler);

	// Starts listening and accepting connections. Returns false (having
	// already printed why) if the port couldn't be bound.
	bool start();

	void broadcast(const std::string& message);

	// Same as broadcast, but only to one connection - for replies that
	// shouldn't go to anyone else (e.g. a room_joined/room_error reply, or
	// a room-scoped broadcast built from a caller-tracked connection set).
	void sendTo(const std::string& connectionId, const std::string& message);
	void broadcastTo(const std::set<std::string>& connectionIds, const std::string& message);

private:
	// The WebSocket callback (set in the constructor) is a thin forwarder
	// to this - same convention as NetworkClient::onMessage/
	// GameServer::onClientMessage - so the actual per-event-type dispatch
	// lives in a named method instead of the lambda body.
	void onClientMessage(const std::shared_ptr<ix::ConnectionState>& connectionState,
		ix::WebSocket& webSocket, const ix::WebSocketMessagePtr& msg);

	ix::WebSocketServer server;
	OpenHandler onOpenHandler;
	CloseHandler onCloseHandler;
	MessageHandler onMessageHandler;

	// Tracks live connections by id so sendTo/broadcastTo can reach a
	// specific connection outside of a callback (getClients() only returns
	// the raw socket set, with no id attached) - populated/erased right
	// alongside the Open/Close events, before handlers are invoked.
	std::unordered_map<std::string, ix::WebSocket*> clientsById;
};
