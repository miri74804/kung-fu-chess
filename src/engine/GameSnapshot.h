#pragma once

#include "../model/Color.h"
#include "../model/PieceType.h"
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
};
