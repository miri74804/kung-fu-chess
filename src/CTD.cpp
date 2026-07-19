// Repo: https://github.com/miri74804/kung-fu-chess
//
// The real, graphical game: parses a board from stdin (same "Board:"
// format as before), then loops - poll for a click, feed it to Controller
// (ignoring clicks while a motion is active - Controller itself stays
// blind to timing, per the architecture), advance game time, render, show.
// ESC closes the window.
#include <iostream>
#include <stdexcept>
#include "io/BoardParser.h"
#include "engine/GameEngine.h"
#include "input/Controller.h"
#include "input/GameWindow.h"
#include "view/PieceGraphicsLibrary.h"
#include "view/Renderer.h"

int main() {
	BoardParser boardParser;
	Board board;

	try {
		board = boardParser.parseBoard();
	}
	catch (const std::runtime_error& e) {
		std::cout << e.what() << "\n";
		return 0;
	}

	GameEngine engine(board);
	Controller controller;
	GameWindow window;

	PieceGraphicsLibrary library;
	library.loadAll(std::string(PROJECT_ROOT_DIR) + "/assets/pieces");
	Renderer renderer(library);

	std::string boardImagePath = std::string(PROJECT_ROOT_DIR) + "/assets/board.png";
	const int TICK_MS = 30;

	std::cout << "Click pieces to move them. Press ESC in the window to quit.\n";

	while (true) {
		int clickX, clickY;
		if (window.pollClick(clickX, clickY) && !engine.isMotionInProgress()) {
			int margin = renderer.marginPx(engine.getBoard().getWidth());
			controller.click(clickX - margin, clickY - margin, engine);
		}

		engine.advanceTime(TICK_MS);

		Img frame = renderer.render(boardImagePath, engine.snapshot(), TICK_MS,
			controller.hasSelection(), controller.getSelectedPosition());
		int key = frame.show(TICK_MS);
		if (key == 27) { // ESC
			break;
		}
	}

	return 0;
}
