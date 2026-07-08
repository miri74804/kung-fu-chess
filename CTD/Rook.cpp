#include "Rook.h"
#include "Board.h"

bool Rook::isValidMove(const Position& from, const Position& to, const Board& board) const {
	int rowDiff = to.row - from.row;
	int colDiff = to.col - from.col;

	if ((rowDiff == 0) == (colDiff == 0)) {
		return false;
	}

	if (!isPathClear(from, to, board)) {
		return false;
	}

	Piece* targetPiece = board.getPieceAt(to);
	if (targetPiece != nullptr && targetPiece->getColor() == this->color) {
		return false;
	}

	return true;
}
