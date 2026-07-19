#pragma once

#include "PieceAnimationState.h"
#include "PieceGraphicsLibrary.h"
#include "../../model/Color.h"
#include "../../model/PieceType.h"

// Tracks one piece's animation: current state, elapsed time, current frame.
// advance(ms) is explicit and deterministic (no real sleep), same model as
// RealTimeArbiter. A state changes externally via setState() (a real game
// event, e.g. a move starting), or internally inside advance() when a
// non-looping state finishes and auto-chains to next_state_when_finished
// (e.g. "jump" -> "short_rest" -> "long_rest" -> "idle").
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
};
