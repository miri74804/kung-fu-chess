#pragma once

#include "PieceAnimationState.h"
#include <string>

// Mirrors one state's config.json: physics (used later by RealTimeArbiter-adjacent
// code for real movement speed) and graphics (used by AnimationState for playback).
struct AnimationConfig {
	double speedMetersPerSec = 0.0;
	PieceAnimationState nextStateWhenFinished = PieceAnimationState::Idle;
	int framesPerSec = 1;
	bool isLoop = true;

	// Parses a config.json file with the fixed shape:
	// { "physics": {...}, "graphics": {...} }
	static AnimationConfig loadFromFile(const std::string& path);
};
