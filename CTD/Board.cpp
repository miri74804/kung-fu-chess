#include "Board.h"
#include <iostream>

Board::Board() : width(0) {}

bool Board::addRow(const std::vector<Token>& row) {
	if (row.empty()) {
		return false;
	}

	int rowWidth = static_cast<int>(row.size()); 

	if (board.empty()) {
		width = rowWidth;
	}
	else if (rowWidth != width) {
		return false;
	}

	board.push_back(row);
	return true;
}

void Board::print() const {
	for (int i = 0; i < getHeight(); ++i) {
		for (int j = 0; j < width; ++j) {
			if (j > 0) {
				std::cout << " ";
			}

			const Token& token = board[i][j];
			if (token.isEmpty()) {
				std::cout << ".";
			} else {
				std::cout << COLOR_SYMBOLS[static_cast<int>(token.color)];
				std::cout << PIECE_SYMBOLS[static_cast<int>(token.piece)];
			}
		}
		std::cout << "\n";
	}
}

bool Board::isValidPosition(const Position& pos) const {
	return pos.row >= 0 && pos.row <  getHeight() &&
		pos.col >= 0 && pos.col < width;  
}

Token Board::getPieceAt(const Position& pos) const {
	if (!isValidPosition(pos)) {
		return Token{ Color::NONE, Piece::NONE };
	}
	return board[pos.row][pos.col];
}

bool Board::movePiece(const Position& from, const Position& to) {

	if (!isValidPosition(from) || !isValidPosition(to)) {
		return false;
	}

	Token piece = getPieceAt(from);
	if (piece.isEmpty()) {
		return false;
	}

	board[from.row][from.col] = Token{ Color::NONE, Piece::NONE };
	board[to.row][to.col] = piece;

	return true;
}

int Board::getWidth() const {
	return width;
}

int Board::getHeight() const {
	return static_cast<int>(board.size());
}
