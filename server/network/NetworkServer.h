#pragma once

#include <ixwebsocket/IXConnectionState.h>
#include <ixwebsocket/IXWebSocketServer.h>
#include <functional>
#include <memory>
#include <string>

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
};
