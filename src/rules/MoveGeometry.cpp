#include "MoveGeometry.h"
#include "../model/Board.h"
#include "../model/Piece.h"
#include <cstdlib>
#include <algorithm>

bool MoveGeometry::isPathClear(const Position& from, const Position& to, const Board& board) {
	int rowDiff = to.row - from.row;
	int colDiff = to.col - from.col;

	int rowStep = (rowDiff == 0) ? 0 : (rowDiff > 0 ? 1 : -1);
	int colStep = (colDiff == 0) ? 0 : (colDiff > 0 ? 1 : -1);

	int currentRow = from.row + rowStep;
	int currentCol = from.col + colStep;

	while (currentRow != to.row || currentCol != to.col) {
		if (board.getPieceAt(Position(currentRow, currentCol)) != nullptr) {
			return false;
		}
		currentRow += rowStep;
		currentCol += colStep;
	}

	return true;
}

bool MoveGeometry::isBlockedBySameColor(const Position& to, const Board& board, const Piece* piece) {
	Piece* targetPiece = board.getPieceAt(to);
	return targetPiece != nullptr && targetPiece->getColor() == piece->getColor();
}

int MoveGeometry::calculateDuration(const Position& source, const Position& target) {
	int rowDiff = std::abs(target.row - source.row);
	int colDiff = std::abs(target.col - source.col);
	int distance = std::max(rowDiff, colDiff);
	const int MOVEMENT_SPEED = 1000;  
	return distance * MOVEMENT_SPEED;
}

bool MoveGeometry::isStraightLine(const Position& from, const Position& to) {
	int rowDiff = to.row - from.row;
	int colDiff = to.col - from.col;
	return (rowDiff == 0) != (colDiff == 0);
}

bool MoveGeometry::isDiagonal(const Position& from, const Position& to) {
	int rowDiff = to.row - from.row;
	int colDiff = to.col - from.col;
	return rowDiff != 0 && std::abs(rowDiff) == std::abs(colDiff);
}
