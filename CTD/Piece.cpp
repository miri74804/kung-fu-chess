#include "Piece.h"
#include "Board.h"

bool Piece::isPathClear(const Position& from, const Position& to, const Board& board) const {
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
