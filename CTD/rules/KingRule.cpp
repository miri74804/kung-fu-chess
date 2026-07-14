#include <cstdlib>
#include "KingRule.h"
#include "../model/Piece.h"
#include "../model/Board.h"

bool KingRule::isValidMove(const Position& from, const Position& to, const Board& board, const Piece* piece) const {
	int rowDiff = std::abs(to.row - from.row);
	int colDiff = std::abs(to.col - from.col);

	if (rowDiff > 1 || colDiff > 1 || (rowDiff == 0 && colDiff == 0)) {
		return false;
	}

	if (piece->isBlockedBySameColor(to, board)) {
		return false;
	}

	return true;
}
