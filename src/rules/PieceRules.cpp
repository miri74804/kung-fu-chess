#include "PieceRules.h"
#include "MoveGeometry.h"
#include "../model/Piece.h"
#include "../model/Board.h"
#include <cstdlib>

bool RookRule::isValidMove(const Position& from, const Position& to, const Board& board) const {
	if (!MoveGeometry::isStraightLine(from, to)) {
		return false;
	}

	if (!MoveGeometry::isPathClear(from, to, board)) {
		return false;
	}

	if (MoveGeometry::isBlockedBySameColor(to, board, board.getPieceAt(from))) {
		return false;
	}

	return true;
}

bool BishopRule::isValidMove(const Position& from, const Position& to, const Board& board) const {
	if (!MoveGeometry::isDiagonal(from, to)) {
		return false;
	}

	if (!MoveGeometry::isPathClear(from, to, board)) {
		return false;
	}

	if (MoveGeometry::isBlockedBySameColor(to, board, board.getPieceAt(from))) {
		return false;
	}

	return true;
}

bool QueenRule::isValidMove(const Position& from, const Position& to, const Board& board) const {
	if (!MoveGeometry::isStraightLine(from, to) && !MoveGeometry::isDiagonal(from, to)) {
		return false;
	}

	if (!MoveGeometry::isPathClear(from, to, board)) {
		return false;
	}

	if (MoveGeometry::isBlockedBySameColor(to, board, board.getPieceAt(from))) {
		return false;
	}

	return true;
}

bool KnightRule::isValidMove(const Position& from, const Position& to, const Board& board) const {
	int rowDiff = std::abs(to.row - from.row);
	int colDiff = std::abs(to.col - from.col);

	if (!((rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2))) {
		return false;
	}

	if (MoveGeometry::isBlockedBySameColor(to, board, board.getPieceAt(from))) {
		return false;
	}

	return true;
}

bool KingRule::isValidMove(const Position& from, const Position& to, const Board& board) const {
	int rowDiff = std::abs(to.row - from.row);
	int colDiff = std::abs(to.col - from.col);

	if (rowDiff > 1 || colDiff > 1 || (rowDiff == 0 && colDiff == 0)) {
		return false;
	}

	if (MoveGeometry::isBlockedBySameColor(to, board, board.getPieceAt(from))) {
		return false;
	}

	return true;
}
