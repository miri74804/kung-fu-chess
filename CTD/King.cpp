#include <cstdlib>
#include "King.h"
#include "Board.h"

bool King::isValidMove(const Position& from, const Position& to, const Board& board) const {
	int rowDiff = std::abs(to.row - from.row);
	int colDiff = std::abs(to.col - from.col);

	if (rowDiff > 1 || colDiff > 1 || (rowDiff == 0 && colDiff == 0)) {
		return false;
	}

	if (isBlockedBySameColor(to, board)) {
		return false;
	}

	return true;
}
