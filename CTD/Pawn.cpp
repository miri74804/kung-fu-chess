#include "Pawn.h"
#include "Board.h"

bool Pawn::isValidMove(const Position& from, const Position& to, const Board& board) const {
	int rowDiff = to.row - from.row;
	int colDiff = to.col - from.col;

	if (rowDiff == 0) {
		return false;
	}

	int expectedDirection = (this->color == Color::White) ? -1 : 1;

	if ((rowDiff > 0 && expectedDirection < 0) || (rowDiff < 0 && expectedDirection > 0)) {
		return false;  
	}

	if (colDiff == 0) {
		Piece* targetPiece = board.getPieceAt(to);
		if (targetPiece != nullptr) {
			return false;  
		}

		if (std::abs(rowDiff) == 1) {
			return true;
		}

		if (std::abs(rowDiff) == 2) {
			int startingRow = (this->color == Color::White) ? board.getHeight() - 2 : 1;
			if (from.row != startingRow) {
				return false;  
			}

			int intermediateRow = from.row + expectedDirection;
			if (board.getPieceAt(Position(intermediateRow, from.col)) != nullptr) {
				return false;   
			}

			return true;
		}

		return false;   
	}

	if (std::abs(colDiff) == 1 && std::abs(rowDiff) == 1) {
		Piece* targetPiece = board.getPieceAt(to);
		if (targetPiece == nullptr || targetPiece->getColor() == this->color) {
			return false;  
		}
		return true;
	}

	return false;  
}
