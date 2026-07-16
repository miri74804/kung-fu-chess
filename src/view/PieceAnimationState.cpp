#include "PieceAnimationState.h"
#include <utility>
#include <iterator>

namespace {
	const std::pair<PieceAnimationState, const char*> STATE_TABLE[] = {
		{ PieceAnimationState::Idle,      "idle" },
		{ PieceAnimationState::Move,      "move" },
		{ PieceAnimationState::Jump,      "jump" },
		{ PieceAnimationState::LongRest,  "long_rest" },
		{ PieceAnimationState::ShortRest, "short_rest" },
	};
}

std::string PieceAnimationStateNotation::toFolderName(PieceAnimationState state) {
	for (int i = 0; i < std::size(STATE_TABLE); ++i) {
		if (STATE_TABLE[i].first == state) {
			return STATE_TABLE[i].second;
		}
	}
	return "idle";
}

PieceAnimationState PieceAnimationStateNotation::parseFolderName(const std::string& name) {
	for (int i = 0; i < std::size(STATE_TABLE); ++i) {
		if (name == STATE_TABLE[i].second) {
			return STATE_TABLE[i].first;
		}
	}
	return PieceAnimationState::Idle;
}
