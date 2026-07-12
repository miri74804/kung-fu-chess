#include "Board.h"
#include "Queen.h" 
#include <iostream>

Board::Board() : width(0) {}

bool Board::addRow(std::vector<std::unique_ptr<Piece>>& row) {
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

	board.push_back(std::move(row));
	return true;
}

void Board::print() const {
	for (int i = 0; i < getHeight(); ++i) {
		for (int j = 0; j < width; ++j) {
			if (j > 0) {
				std::cout << " ";
			}

			const auto& piece = board[i][j];
			if (piece == nullptr) {
				std::cout << ".";
			} else {
				std::cout << COLOR_SYMBOLS[static_cast<int>(piece->getColor())];
				std::cout << piece->getSymbol();
			}
		}
		std::cout << "\n";
	}
}

bool Board::isValidPosition(const Position& pos) const {
	return pos.row >= 0 && pos.row < getHeight() &&
		pos.col >= 0 && pos.col < width;  
}

Piece* Board::getPieceAt(const Position& pos) const {
	if (!isValidPosition(pos)) {
		return nullptr;
	}
	return board[pos.row][pos.col].get();
}

bool Board::validateMove(const Position& from, const Position& to) {
	if (!isValidPosition(to)) {
		return false;
	}

	Piece* piece = getPieceAt(from);
	if (piece == nullptr) {
		return false;
	}

	if (!piece->isValidMove(from, to, *this)) {
		return false;
	}

	return true;
}

void Board::movePieceOnBoard(const Position& from, const Position& to) {
	board[to.row][to.col] = std::move(board[from.row][from.col]);
}

void Board::removePieceAt(const Position& pos) {
	if (!isValidPosition(pos)) {
		return;
	}
	board[pos.row][pos.col] = nullptr;
}

void Board::promoteToQueen(const Position& pos) {
	Piece* piece = getPieceAt(pos);
	if (piece == nullptr) {
		return;
	}

	board[pos.row][pos.col] = std::make_unique<Queen>(piece->getColor());
}

int Board::getWidth() const {
	return width;
}

int Board::getHeight() const {
	return static_cast<int>(board.size());
}
