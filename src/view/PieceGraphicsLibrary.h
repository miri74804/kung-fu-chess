#pragma once

#include "PieceSprite.h"
#include "../model/Color.h"
#include "../model/PieceType.h"
#include <map>
#include <string>

// Loads and owns every {color}{type}/states/{state} sprite set from
// assets/pieces/ once at startup, so the Renderer can just ask for what
// it needs by (color, type, state) instead of touching the filesystem.
class PieceGraphicsLibrary {
public:
	// assetsPiecesDir is the path to the "pieces" folder itself, e.g. ".../assets/pieces".
	void loadAll(const std::string& assetsPiecesDir);

	const PieceSprite& get(Color color, PieceType type, PieceAnimationState state) const;

private:
	std::map<std::string, PieceSprite> sprites;

	static std::string makeKey(Color color, PieceType type, PieceAnimationState state);
};
