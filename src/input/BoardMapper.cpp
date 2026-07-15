#include "BoardMapper.h"
#include "../model/Board.h"
#include "../model/Position.h"

namespace {
	const int CELL_SIZE = 100;
}

bool BoardMapper::pixelToCell(int x, int y, const Board& board, Position& outPosition) {
	Position candidate(y / CELL_SIZE, x / CELL_SIZE);

	if (!board.isValidPosition(candidate)) {
		return false;
	}

	outPosition = candidate;
	return true;
}
