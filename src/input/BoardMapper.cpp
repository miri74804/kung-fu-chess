#include "BoardMapper.h"
#include "../model/Position.h"
#include "../Constants.h"

bool BoardMapper::pixelToCell(int x, int y, int boardWidth, int boardHeight, Position& outPosition) {
	// Negative x/y (a click above/left of the board) must be rejected
	// before dividing - C++ integer division truncates toward zero, not
	// floor, so e.g. -5 / CELL_SIZE is 0, not -1. Without this check, a
	// click in the ~CELL_SIZE-wide margin just above/left of the board
	// would wrongly land on row/col 0 instead of being out of bounds.
	if (x < 0 || y < 0) {
		return false;
	}

	Position candidate(y / CELL_SIZE, x / CELL_SIZE);

	if (candidate.row >= boardHeight || candidate.col >= boardWidth) {
		return false;
	}

	outPosition = candidate;
	return true;
}
