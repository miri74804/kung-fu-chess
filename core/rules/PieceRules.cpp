#include "PieceRules.h"
#include "MoveGeometry.h"
#include "../model/Piece.h"
#include "../model/Board.h"
#include <cstdlib>

namespace {
	bool isClearSlidingMove(const Position& from, const Position& to, const Board& board) {
		if (!MoveGeometry::isPathClear(from, to, board)) {
			return false;
		}
		return !MoveGeometry::isBlockedBySameColor(to, board, board.getPieceAt(from));
	}
}

bool RookRule::isValidMove(const Position& from, const Position& to, const Board& board) const {
	return MoveGeometry::isStraightLine(from, to) && isClearSlidingMove(from, to, board);
}

bool BishopRule::isValidMove(const Position& from, const Position& to, const Board& board) const {
	return MoveGeometry::isDiagonal(from, to) && isClearSlidingMove(from, to, board);
}

bool QueenRule::isValidMove(const Position& from, const Position& to, const Board& board) const {
	bool movesLikeRookOrBishop = MoveGeometry::isStraightLine(from, to) || MoveGeometry::isDiagonal(from, to);
	return movesLikeRookOrBishop && isClearSlidingMove(from, to, board);
}

bool KnightRule::isValidMove(const Position& from, const Position& to, const Board& board) const {
	int rowDiff = std::abs(to.row - from.row);
	int colDiff = std::abs(to.col - from.col);
	bool movesInLShape = (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);

	return movesInLShape && !MoveGeometry::isBlockedBySameColor(to, board, board.getPieceAt(from));
}

bool KingRule::isValidMove(const Position& from, const Position& to, const Board& board) const {
	int rowDiff = std::abs(to.row - from.row);
	int colDiff = std::abs(to.col - from.col);
	bool movesOneSquare = rowDiff <= 1 && colDiff <= 1 && (rowDiff != 0 || colDiff != 0);

	return movesOneSquare && !MoveGeometry::isBlockedBySameColor(to, board, board.getPieceAt(from));
}
