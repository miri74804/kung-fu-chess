#include "Rook.h"
#include "Board.h"

bool Rook::isValidMove(const Position& from, const Position& to, const Board& board) const {
	int rowDiff = to.row - from.row;
	int colDiff = to.col - from.col;

	if ((rowDiff == 0) == (colDiff == 0)) {
		return false;
	}

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

	Piece* targetPiece = board.getPieceAt(to);
	if (targetPiece != nullptr && targetPiece->getColor() == this->color) {
		return false;
	}

	return true;
}
