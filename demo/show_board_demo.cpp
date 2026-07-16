// Minimal smoke test for the Img class: loads assets/board.png and shows it
// in a window. Not part of the game itself — just proof that Img + OpenCV
// are wired correctly before building the real Renderer.
#include "../src/view/Img.h"
#include <iostream>

int main() {
	Img board;
	try {
		board.read(std::string(PROJECT_ROOT_DIR) + "/assets/board.png");
		std::cout << "Loaded board.png: " << board.get_mat().cols << "x" << board.get_mat().rows << " pixels\n";
		std::cout << "Press any key in the image window to close it...\n";
		board.show();
	}
	catch (const std::exception& e) {
		std::cerr << "Failed to load board: " << e.what() << "\n";
		return 1;
	}
	return 0;
}
