#pragma once

#include "PieceAnimationState.h"
#include "PieceGraphicsLibrary.h"
#include "../model/Color.h"
#include "../model/PieceType.h"

// Tracks playback for one piece: current animation state, elapsed time
// within it, and the resulting frame to draw. advance(ms) is explicit and
// deterministic (no real sleep), matching RealTimeArbiter's own model so
// this stays testable without a real clock.
//
// Two ways a state changes:
//  - externally, via setState() - e.g. GameEngine starts a real move, or
//    RealTimeArbiter reports arrival ("move" -> "long_rest").
//  - internally, inside advance() - a non-looping state (is_loop: false)
//    that finishes playing its 5 frames auto-chains to its
//    next_state_when_finished (e.g. "jump" -> "short_rest" -> "long_rest" -> "idle").
class AnimationState {
public:
	AnimationState(const PieceGraphicsLibrary& library, Color color, PieceType type);

	void advance(int ms);
	void setState(PieceAnimationState state);

	PieceAnimationState getState() const { return state; }
	const Img& getCurrentFrame() const;

private:
	const PieceGraphicsLibrary& library;
	Color color;
	PieceType type;
	PieceAnimationState state;
	int elapsedMs;

	const PieceSprite& currentSprite() const;
};
