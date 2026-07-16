#pragma once

#include <string>

// The 5 animation states every piece's sprite sheet is organized under
// (assets/pieces/{color}{type}/states/{state}/). Distinct from Piece::state
// (idle/moving/captured) in the model — this is view-layer only.
enum class PieceAnimationState {
	Idle,
	Move,
	Jump,
	LongRest,
	ShortRest
};

class PieceAnimationStateNotation {
public:
	static std::string toFolderName(PieceAnimationState state);
	static PieceAnimationState parseFolderName(const std::string& name);
};
