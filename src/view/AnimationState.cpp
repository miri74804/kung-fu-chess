#include "AnimationState.h"
#include <algorithm>
#include <stdexcept>

namespace {
	int durationMsOf(const PieceSprite& sprite) {
		int frameCount = static_cast<int>(sprite.frames.size());
		if (sprite.config.framesPerSec <= 0) {
			return 0;
		}
		return (frameCount * 1000) / sprite.config.framesPerSec;
	}
}

AnimationState::AnimationState(const PieceGraphicsLibrary& lib, Color c, PieceType t)
	: library(lib), color(c), type(t), state(PieceAnimationState::Idle), elapsedMs(0) {}

const PieceSprite& AnimationState::currentSprite() const {
	return library.get(color, type, state);
}

void AnimationState::setState(PieceAnimationState newState) {
	state = newState;
	elapsedMs = 0;
}

void AnimationState::advance(int ms) {
	elapsedMs += ms;

	// Chain through finished non-looping states (carrying over the leftover
	// time), stopping once we land on a looping state or one whose next
	// state is itself.
	while (true) {
		const PieceSprite& sprite = currentSprite();
		int durationMs = durationMsOf(sprite);

		if (sprite.config.isLoop || durationMs <= 0 || elapsedMs < durationMs) {
			break;
		}

		PieceAnimationState nextState = sprite.config.nextStateWhenFinished;
		elapsedMs -= durationMs;

		if (nextState == state) {
			elapsedMs = 0;
			break;
		}
		state = nextState;
	}
}

const Img& AnimationState::getCurrentFrame() const {
	const PieceSprite& sprite = currentSprite();
	int frameCount = static_cast<int>(sprite.frames.size());
	if (frameCount == 0) {
		throw std::runtime_error("No frames loaded for the current animation state");
	}

	int fps = sprite.config.framesPerSec > 0 ? sprite.config.framesPerSec : 1;
	int frameIndex = (elapsedMs * fps) / 1000;

	if (sprite.config.isLoop) {
		frameIndex %= frameCount;
	}
	else {
		frameIndex = std::min(frameIndex, frameCount - 1);
	}

	return sprite.frames[frameIndex];
}
