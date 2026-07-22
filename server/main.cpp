// Minimal WebSocket echo server - a build/toolchain smoke test only.
// The real game protocol (GameEngine ownership, move commands, snapshot
// broadcast) is not wired up yet.
#include <ixwebsocket/IXWebSocketServer.h>
#include <ixwebsocket/IXNetSystem.h>
#include <iostream>

int main() {
	// On Windows, socket calls (including the setsockopt inside listen())
	// fail until WSAStartup has run - IXWebSocket doesn't call it for you.
	ix::initNetSystem();

	ix::WebSocketServer server(8080);

	server.setOnClientMessageCallback(
		[](std::shared_ptr<ix::ConnectionState>, ix::WebSocket& webSocket, const ix::WebSocketMessagePtr& msg) {
			if (msg->type == ix::WebSocketMessageType::Open) {
				std::cout << "Client connected\n";
			}
			else if (msg->type == ix::WebSocketMessageType::Message) {
				std::cout << "Received: " << msg->str << "\n";
				webSocket.send(msg->str);
			}
		});

	auto result = server.listen();
	if (!result.first) {
		std::cerr << result.second << "\n";
		ix::uninitNetSystem();
		return 1;
	}

	server.start();
	std::cout << "Server listening on port 8080\n";
	server.wait();

	ix::uninitNetSystem();
	return 0;
}
