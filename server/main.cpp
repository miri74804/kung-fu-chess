// Headless server entry point: parses the board from stdin (same
// "Board:" format the old single-process client used), then hands off to
// GameServer, which owns the GameEngine and the WebSocket connections.
#include "GameServer.h"
#include "../core/io/BoardParser.h"
#include <cstdio>
#include <ixwebsocket/IXNetSystem.h>
#include <iostream>
#include <utility>

namespace {
	constexpr int PORT = 8080;
}

int main() {
	// Unbuffered, so "Created room"/"Assigned .../etc" show up immediately
	// even when stdout is redirected to a file/pipe (fully buffered by
	// default in that case) - useful for watching what's happening live.
	std::setvbuf(stdout, nullptr, _IONBF, 0);

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

	GameServer server(std::move(board), PORT);
	server.run();

	ix::uninitNetSystem();
	return 0;
}
