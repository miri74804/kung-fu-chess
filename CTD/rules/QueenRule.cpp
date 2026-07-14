#include <cstdlib>
#include "QueenRule.h"
#include "../model/Piece.h"
#include "../model/Board.h"

bool QueenRule::isValidMove(const Position& from, const Position& to, const Board& board, const Piece* piece) const {
	int rowDiff = to.row - from.row;
	int colDiff = to.col - from.col;

	if (rowDiff == 0 && colDiff == 0) {
		return false;
	}

	bool isStraight = (rowDiff == 0) != (colDiff == 0);
	bool isDiagonal = (std::abs(rowDiff) == std::abs(colDiff));

	if (!isStraight && !isDiagonal) {
		return false;
	}

	if (!piece->isPathClear(from, to, board)) {
		return false;
	}

	if (piece->isBlockedBySameColor(to, board)) {
		return false;
	}

	return true;
}
