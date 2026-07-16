// Smoke test for PieceGraphicsLibrary: loads all 12 pieces x 5 states from
// assets/pieces/ and reports success. No window - safe to run headlessly.
#include "../src/view/PieceGraphicsLibrary.h"
#include "../src/model/Color.h"
#include "../src/model/PieceType.h"
#include <iostream>

int main() {
	PieceGraphicsLibrary library;
	try {
		library.loadAll(std::string(PROJECT_ROOT_DIR) + "/assets/pieces");
	}
	catch (const std::exception& e) {
		std::cerr << "Failed to load piece graphics: " << e.what() << "\n";
		return 1;
	}

	const PieceSprite& whitePawnMove = library.get(Color::White, PieceType::PAWN, PieceAnimationState::Move);
	std::cout << "White pawn 'move': " << whitePawnMove.frames.size() << " frames, "
		<< whitePawnMove.config.framesPerSec << " fps, speed="
		<< whitePawnMove.config.speedMetersPerSec << " m/s\n";

	const PieceSprite& blackKingIdle = library.get(Color::Black, PieceType::KING, PieceAnimationState::Idle);
	std::cout << "Black king 'idle': " << blackKingIdle.frames.size() << " frames, "
		<< blackKingIdle.config.framesPerSec << " fps\n";

	std::cout << "All piece/state combinations loaded successfully.\n";
	return 0;
}
