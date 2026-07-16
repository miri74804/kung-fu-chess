// Smoke test for AnimationState: proves the automatic state chain
// (jump -> short_rest -> long_rest -> idle) advances correctly using
// explicit advance(ms) steps - no real sleep involved.
#include "../src/view/PieceGraphicsLibrary.h"
#include "../src/view/AnimationState.h"
#include "../src/model/Color.h"
#include "../src/model/PieceType.h"
#include <iostream>

namespace {
	const char* stateName(PieceAnimationState s) {
		switch (s) {
		case PieceAnimationState::Idle: return "idle";
		case PieceAnimationState::Move: return "move";
		case PieceAnimationState::Jump: return "jump";
		case PieceAnimationState::LongRest: return "long_rest";
		case PieceAnimationState::ShortRest: return "short_rest";
		}
		return "?";
	}
}

int main() {
	PieceGraphicsLibrary library;
	library.loadAll(std::string(PROJECT_ROOT_DIR) + "/assets/pieces");

	AnimationState anim(library, Color::White, PieceType::PAWN);
	anim.setState(PieceAnimationState::Jump);

	// wP durations: jump=500ms, short_rest=~833ms, long_rest=2500ms, then idle forever.
	int checkpoints[] = { 0, 100, 499, 501, 1000, 1333, 1334, 3833, 3834, 10000 };
	int previousMs = 0;

	for (int targetMs : checkpoints) {
		anim.advance(targetMs - previousMs);
		previousMs = targetMs;
		std::cout << "t=" << targetMs << "ms -> state=" << stateName(anim.getState()) << "\n";
	}

	std::cout << "Done.\n";
	return 0;
}
