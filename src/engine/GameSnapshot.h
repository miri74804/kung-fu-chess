#pragma once

#include "../model/Color.h"
#include "../model/PieceType.h"
#include "../model/Position.h"
#include <vector>

// One piece's read-only view-model data: what to draw and where.
// row/col are in board-cell units, not pixels - a piece mid-flight has a
// fractional value (e.g. row=3.5) interpolated between its source and
// destination. Converting to pixels is the Renderer's job (it owns CELL_SIZE).
struct PieceSnapshot {
	Color color;
	PieceType type;
	double row;
	double col;
};

// Read-only view of the game, produced by GameEngine::snapshot(). The
// Renderer consumes only this - never a live Board/Piece - so drawing can
// never accidentally mutate game state.
struct GameSnapshot {
	int boardWidth;
	int boardHeight;
	bool gameOver;
	std::vector<PieceSnapshot> pieces;

	// Cells the currently active move passes through (source excluded,
	// destination included), for the Renderer to highlight. Empty when no
	// motion is active. A sliding move (rook/bishop/queen) lists every
	// intermediate cell; a jump-like move (knight) lists only the
	// destination, since there's no real "path" between the two.
	std::vector<Position> activeMovePath;
};
