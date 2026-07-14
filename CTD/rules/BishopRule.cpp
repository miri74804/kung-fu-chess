#include <cstdlib>
#include "BishopRule.h"
#include "../model/Piece.h"
#include "../model/Board.h"

bool BishopRule::isValidMove(const Position& from, const Position& to, const Board& board, const Piece* piece) const {
	int rowDiff = to.row - from.row;
	int colDiff = to.col - from.col;

	if (std::abs(rowDiff) != std::abs(colDiff) || (rowDiff == 0 && colDiff == 0)) {
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
