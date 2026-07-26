#include "MoveGeometry.h"
#include "../model/Board.h"
#include "../model/Piece.h"
#include "../Constants.h"
#include <cstdlib>
#include <algorithm>

namespace {
	void stepDirection(const Position& from, const Position& to, int& rowStep, int& colStep) {
		int rowDiff = to.row - from.row;
		int colDiff = to.col - from.col;
		rowStep = (rowDiff == 0) ? 0 : (rowDiff > 0 ? 1 : -1);
		colStep = (colDiff == 0) ? 0 : (colDiff > 0 ? 1 : -1);
	}
}

bool MoveGeometry::isPathClear(const Position& from, const Position& to, const Board& board) {
	int rowStep, colStep;
	stepDirection(from, to, rowStep, colStep);

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
	return distance * MOVE_DURATION_PER_CELL_MS;
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

std::vector<Position> MoveGeometry::computePath(const Position& source, const Position& destination) {
	if (!isStraightLine(source, destination) && !isDiagonal(source, destination)) {
		// A knight-like jump has no real path between the squares - just
		// report the destination.
		return { destination };
	}

	int rowStep, colStep;
	stepDirection(source, destination, rowStep, colStep);

	int rowDiff = std::abs(destination.row - source.row);
	int colDiff = std::abs(destination.col - source.col);
	int distance = std::max(rowDiff, colDiff);

	std::vector<Position> path;
	for (int step = 1; step <= distance; ++step) {
		path.emplace_back(source.row + rowStep * step, source.col + colStep * step);
	}
	return path;
}
