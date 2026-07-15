#include "PieceRules.h"
#include "MoveGeometry.h"
#include "../model/Piece.h"
#include "../model/Board.h"
#include <cstdlib>

namespace {
	bool isForwardDirection(Color color, int rowDiff) {
		int expectedDirection = (color == Color::White) ? -1 : 1;
		return (rowDiff > 0) == (expectedDirection > 0);
	}

	bool isValidStraightAdvance(const Position& from, const Position& to, const Board& board) {
		if (board.getPieceAt(to) != nullptr) {
			return false;
		}

		int rowDiff = to.row - from.row;

		if (std::abs(rowDiff) == 1) {
			return true;
		}

		if (std::abs(rowDiff) == 2) {
			const Piece* piece = board.getPieceAt(from);
			bool onStartRow = (piece->getColor() == Color::White)
				? (from.row == board.getHeight() - 1)
				: (from.row == 0);

			if (!onStartRow) {
				return false;
			}

			return MoveGeometry::isPathClear(from, to, board);
		}

		return false;
	}

	bool isValidDiagonalCapture(const Position& to, const Board& board, const Piece* piece) {
		Piece* targetPiece = board.getPieceAt(to);
		return targetPiece != nullptr && targetPiece->getColor() != piece->getColor();
	}
}

bool PawnRule::isValidMove(const Position& from, const Position& to, const Board& board) const {
	const Piece* piece = board.getPieceAt(from);
	int rowDiff = to.row - from.row;
	int colDiff = to.col - from.col;

	if (rowDiff == 0) {
		return false;
	}

	if (!isForwardDirection(piece->getColor(), rowDiff)) {
		return false;
	}

	if (colDiff == 0) {
		return isValidStraightAdvance(from, to, board);
	}

	if (std::abs(colDiff) == 1 && std::abs(rowDiff) == 1) {
		return isValidDiagonalCapture(to, board, piece);
	}

	return false;
}
