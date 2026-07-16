// Smoke test for Renderer: parses a board from stdin (same "Board:" format
// the real game uses) and shows the rendered board+pieces in a window.
#include "../src/io/BoardParser.h"
#include "../src/engine/GameEngine.h"
#include "../src/view/PieceGraphicsLibrary.h"
#include "../src/view/Renderer.h"
#include <iostream>

int main() {
	BoardParser parser;
	Board board = parser.parseBoard();
	GameEngine engine(board);

	PieceGraphicsLibrary library;
	library.loadAll(std::string(PROJECT_ROOT_DIR) + "/assets/pieces");

	Renderer renderer(library);
	Img frame = renderer.render(std::string(PROJECT_ROOT_DIR) + "/assets/board.png", engine.snapshot());

	std::cout << "Rendered " << frame.get_mat().cols << "x" << frame.get_mat().rows
		<< ". Press any key in the image window to close it...\n";
	frame.show();

	return 0;
}
