#include "BoardMapper.h"
#include "../model/Board.h"
#include "../model/Position.h"

namespace {
	const int CELL_SIZE = 100;
}

bool BoardMapper::pixelToCell(int x, int y, const Board& board, Position& outPosition) {
	// Negative x/y (a click above/left of the board) must be rejected
	// before dividing - C++ integer division truncates toward zero, not
	// floor, so e.g. -5 / CELL_SIZE is 0, not -1. Without this check, a
	// click in the ~CELL_SIZE-wide margin just above/left of the board
	// would wrongly land on row/col 0 instead of being out of bounds.
	if (x < 0 || y < 0) {
		return false;
	}

	Position candidate(y / CELL_SIZE, x / CELL_SIZE);

	if (!board.isValidPosition(candidate)) {
		return false;
	}

	outPosition = candidate;
	return true;
}
