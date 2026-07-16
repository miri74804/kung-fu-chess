// First live, playable slice: parses a board, then loops forever -
// poll for a click, feed it to the existing Controller (unchanged!),
// advance game time, render, show. ESC closes the window.
#include "../src/io/BoardParser.h"
#include "../src/engine/GameEngine.h"
#include "../src/input/Controller.h"
#include "../src/input/GameWindow.h"
#include "../src/view/PieceGraphicsLibrary.h"
#include "../src/view/Renderer.h"
#include <iostream>

int main() {
	BoardParser parser;
	Board board = parser.parseBoard();

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
		if (window.pollClick(clickX, clickY)) {
			int margin = renderer.marginPx(engine.getBoard().getWidth());
			controller.click(clickX - margin, clickY - margin, engine);
		}

		engine.advanceTime(TICK_MS);

		Img frame = renderer.render(boardImagePath, engine.snapshot());
		int key = frame.show(TICK_MS);
		if (key == 27) { // ESC
			break;
		}
	}

	return 0;
}
