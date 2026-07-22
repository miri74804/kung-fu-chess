#include "PieceAnimationState.h"
#include "../../utils/EnumLookup.h"
#include <utility>
#include <stdexcept>

namespace {
	const std::pair<PieceAnimationState, std::string> STATE_TABLE[] = {
		{ PieceAnimationState::Idle,      "idle" },
		{ PieceAnimationState::Move,      "move" },
		{ PieceAnimationState::Jump,      "jump" },
		{ PieceAnimationState::LongRest,  "long_rest" },
		{ PieceAnimationState::ShortRest, "short_rest" },
	};
}

std::string PieceAnimationStateNotation::toFolderName(PieceAnimationState state) {
	auto name = lookupValue(STATE_TABLE, state);
	if (!name.has_value()) {
		throw std::runtime_error("Unknown PieceAnimationState value");
	}
	return *name;
}

PieceAnimationState PieceAnimationStateNotation::parseFolderName(const std::string& name) {
	auto state = lookupKey(STATE_TABLE, name);
	if (!state.has_value()) {
		throw std::runtime_error("Unknown animation state folder name: " + name);
	}
	return *state;
}
