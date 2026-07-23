// Headless server entry point: parses the board from stdin (same
// "Board:" format the old single-process client used), then hands off to
// GameServer, which owns the GameEngine and the WebSocket connections.
#include "GameServer.h"
#include "../src/io/BoardParser.h"
#include <ixwebsocket/IXNetSystem.h>
#include <iostream>

namespace {
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

	GameServer server(board, PORT);
	server.run();

	ix::uninitNetSystem();
	return 0;
}
