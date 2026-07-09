#include <cstdlib>
#include "Knight.h"
#include "Board.h"

bool Knight::isValidMove(const Position& from, const Position& to, const Board& board) const {
	int rowDiff = std::abs(to.row - from.row);
	int colDiff = std::abs(to.col - from.col);

	if (!((rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2))) {
		return false;
	}

	if (isBlockedBySameColor(to, board)) {
		return false;
	}

	return true;
}
