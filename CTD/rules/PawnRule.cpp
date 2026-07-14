#include <cstdlib>
#include "PawnRule.h"
#include "../model/Piece.h"
#include "../model/Board.h"

bool PawnRule::isValidMove(const Position& from, const Position& to, const Board& board, const Piece* piece) const {
	int rowDiff = to.row - from.row;
	int colDiff = to.col - from.col;

	if (rowDiff == 0) {
		return false;
	}

	int expectedDirection = (piece->getColor() == Color::White) ? -1 : 1;

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
			if (piece->getColor() == Color::White) {
				if (from.row != board.getHeight() - 1) {
					return false;
				}
			}
			else {
				if (from.row != 0) {
					return false;
				}
			}

			if (!piece->isPathClear(from, to, board)) {
				return false;
			}

			return true;
		}

		return false;
	}

	if (std::abs(colDiff) == 1 && std::abs(rowDiff) == 1) {
		Piece* targetPiece = board.getPieceAt(to);
		if (targetPiece == nullptr || targetPiece->getColor() == piece->getColor()) {
			return false;
		}
		return true;
	}

	return false;
}
