#pragma once

#include "PieceAnimationState.h"
#include <string>

struct AnimationConfig {
	double speedMetersPerSec = 0.0;
	PieceAnimationState nextStateWhenFinished = PieceAnimationState::Idle;
	int framesPerSec = 1;
	bool isLoop = true;

	static AnimationConfig loadFromFile(const std::string& path);
};
