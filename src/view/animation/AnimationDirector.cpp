#include "AnimationDirector.h"
#include "../../Constants.h"
#include <cmath>

AnimationDirector::AnimationDirector(const PieceGraphicsLibrary& lib) : library(lib) {}

bool AnimationDirector::isFractional(const PieceSnapshot& piece) {
	double rowRounded = std::round(piece.row);
	double colRounded = std::round(piece.col);
	return std::abs(piece.row - rowRounded) > FRACTIONAL_EPSILON
		|| std::abs(piece.col - colRounded) > FRACTIONAL_EPSILON;
}

void AnimationDirector::advance(int ms, const GameSnapshot& snapshot) {
	const PieceSnapshot* mover = nullptr;
	for (const PieceSnapshot& piece : snapshot.pieces) {
		if (isFractional(piece)) {
			mover = &piece;
			break;
		}
	}

	if (mover != nullptr) {
		if (!moverClock.has_value()) {
			moverClock.emplace(library, mover->color, mover->type);
			moverClock->setState(PieceAnimationState::Move);
		}
		moverClock->advance(ms);
	}
	else {
		moverClock.reset();
	}

	for (const PieceSnapshot& piece : snapshot.pieces) {
		std::pair<Color, PieceType> key(piece.color, piece.type);
		auto it = idleClocks.find(key);
		if (it == idleClocks.end()) {
			it = idleClocks.emplace(key, AnimationState(library, piece.color, piece.type)).first;
		}
		it->second.advance(ms);
	}
}

const Img& AnimationDirector::frameFor(const PieceSnapshot& piece) const {
	if (moverClock.has_value() && isFractional(piece)) {
		return moverClock->getCurrentFrame();
	}

	std::pair<Color, PieceType> key(piece.color, piece.type);
	return idleClocks.at(key).getCurrentFrame();
}
