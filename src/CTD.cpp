// Repo: https://github.com/miri74804/kung-fu-chess
//
// Entry point: parses a board from stdin (same "Board:" format as before),
// then hands off to Game, which owns the engine/input/rendering and runs
// the live loop until the window is closed.
#include <iostream>
#include <stdexcept>
#include <windows.h>
#include "io/BoardParser.h"
#include "Game.h"

int main() {
	// Without this, Windows silently rescales our window if display scaling
	// (e.g. 125%/150%, common on high-DPI screens) is on - the image then
	// gets stretched on screen and click coordinates stop lining up with
	// its actual pixels (an edge click can land in the next cell over).
	// This just tells Windows "I'll handle my own pixels" - unrelated to
	// Img/drawing rules, it's a one-time OS-level process setting, so it
	// belongs here rather than inside Game.
	SetProcessDPIAware();

	BoardParser boardParser;
	Board board;

	try {
		board = boardParser.parseBoard();
	}
	catch (const std::runtime_error& e) {
		std::cout << e.what() << "\n";
		return 0;
	}

	Game game(std::move(board));
	return game.run();
}
