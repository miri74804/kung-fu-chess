#include <cstdlib>
#include <algorithm>
#include "Piece.h"
#include "Board.h"

Piece::Piece(Color c, PieceType t) 
	: color(c), type(t), movementSpeed(1000), isFirstMove(true) {}

char Piece::getSymbol() const {
	switch (type) {
	case PieceType::KING:
		return 'K';
	case PieceType::QUEEN:
		return 'Q';
	case PieceType::ROOK:
		return 'R';
	case PieceType::BISHOP:
		return 'B';
	case PieceType::KNIGHT:
		return 'N';
	case PieceType::PAWN:
		return 'P';
	default:
		return '.';
	}
}

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

bool Piece::isBlockedBySameColor(const Position& to, const Board& board) const {
	Piece* targetPiece = board.getPieceAt(to);
	return targetPiece != nullptr && targetPiece->getColor() == this->color;
}

int Piece::calculateDuration(const Position& source, const Position& target) const {
	int rowDiff = std::abs(target.row - source.row);
	int colDiff = std::abs(target.col - source.col);
	int distance = std::max(rowDiff, colDiff);
	return distance * movementSpeed;
}
