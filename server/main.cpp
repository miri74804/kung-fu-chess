// Headless server: parses the board from stdin (same "Board:" format as
// the old single-process client), owns the one GameEngine, and speaks
// WebSocket to however many clients connect - broadcasting a GameSnapshot
// every tick and applying whatever move commands arrive. No OpenCV, no
// window: this process never draws anything.
#include "../src/engine/GameEngine.h"
#include "../src/io/BoardParser.h"
#include "../src/protocol/Protocol.h"
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocketServer.h>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

namespace {
	constexpr int TICK_MS = 30;
	constexpr int PORT = 8080;
}

int main() {
	ix::initNetSystem();

	BoardParser boardParser;
	Board board;
	try {
		board = boardParser.parseBoard();
	}
	catch (const std::runtime_error& e) {
		std::cout << e.what() << "\n";
		ix::uninitNetSystem();
		return 0;
	}

	GameEngine engine(board);
	std::mutex engineMutex;

	ix::WebSocketServer server(PORT);

	server.setOnClientMessageCallback(
		[&engine, &engineMutex](std::shared_ptr<ix::ConnectionState>, ix::WebSocket&, const ix::WebSocketMessagePtr& msg) {
			if (msg->type == ix::WebSocketMessageType::Open) {
				std::cout << "Client connected\n";
			}
			else if (msg->type == ix::WebSocketMessageType::Close) {
				std::cout << "Client disconnected\n";
			}
			else if (msg->type == ix::WebSocketMessageType::Message) {
				Protocol::MoveCommand command = Protocol::decodeMoveCommand(msg->str);
				if (command.isValid) {
					std::lock_guard<std::mutex> lock(engineMutex);
					engine.requestMove(command.source, command.destination);
				}
			}
		});

	auto listenResult = server.listen();
	if (!listenResult.first) {
		std::cerr << listenResult.second << "\n";
		ix::uninitNetSystem();
		return 1;
	}

	server.start();
	std::cout << "Server listening on port " << PORT << "\n";

	auto lastTick = std::chrono::steady_clock::now();
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(TICK_MS));

		auto now = std::chrono::steady_clock::now();
		int elapsedMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick).count());
		lastTick = now;

		std::string message;
		{
			std::lock_guard<std::mutex> lock(engineMutex);
			engine.advanceTime(elapsedMs);
			message = Protocol::encodeSnapshot(engine.snapshot());
		}

		for (const std::shared_ptr<ix::WebSocket>& client : server.getClients()) {
			client->send(message);
		}
	}
}
