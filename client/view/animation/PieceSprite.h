#pragma once

#include "AnimationConfig.h"
#include "../Img.h"
#include <vector>

// One piece's one animation state: its timing/physics config plus the 5
// loaded frames (assets/pieces/{color}{type}/states/{state}/sprites/1..5.png).
struct PieceSprite {
	AnimationConfig config;
	std::vector<Img> frames;
};
