#pragma once

#include <string>

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
