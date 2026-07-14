#include <cstdlib>
#include "KnightRule.h"
#include "MoveValidator.h"
#include "../model/Piece.h"
#include "../model/Board.h"

bool KnightRule::isValidMove(const Position& from, const Position& to, const Board& board, const Piece* piece) const {
	int rowDiff = std::abs(to.row - from.row);
	int colDiff = std::abs(to.col - from.col);

	if (!((rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2))) {
		return false;
	}

	if (MoveValidator::isBlockedBySameColor(to, board, piece)) {
		return false;
	}

	return true;
}
