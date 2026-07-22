#pragma once

#include "Img.h"

// Draws file letters (a, b, c, ...) above and below the grid, and rank
// numbers (boardHeight down to 1) to its left and right, each centered in
// the BOARD_MARGIN-wide strip already reserved around the grid - no extra
// space needed. Drawn once into Renderer's cached base canvas (not per
// frame), since the board's size never changes mid-game.
class CoordinateLabels {
public:
	static void draw(Img& target, int boardOffsetX, int boardOffsetY,
		int gridWidthPx, int gridHeightPx, int boardWidth, int boardHeight);
};
