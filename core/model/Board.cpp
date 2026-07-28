#include "Board.h"
#include <iostream>

Board::Board() : width(0), height(0) {}

Board Board::clone() const {
	Board copy;
	for (const std::vector<std::unique_ptr<Piece>>& row : board) {
		std::vector<std::unique_ptr<Piece>> rowCopy;
		rowCopy.reserve(row.size());
		for (const std::unique_ptr<Piece>& piece : row) {
			rowCopy.push_back(piece ? std::make_unique<Piece>(piece->getColor(), piece->getType()) : nullptr);
		}
		copy.addRow(rowCopy);
	}
	return copy;
}

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
	height++;
	return true;
}

Piece* Board::getPieceAt(const Position& pos) const {
	if (!isValidPosition(pos)) {
		return nullptr;
	}
	return board[pos.row][pos.col].get();
}

bool Board::isValidPosition(const Position& pos) const {
	return pos.row >= 0 && pos.row < height &&
		pos.col >= 0 && pos.col < width;  
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

void Board::setPieceAt(const Position& pos, std::unique_ptr<Piece> newPiece) {
	if (!isValidPosition(pos)) {
		return;
	}
	board[pos.row][pos.col] = std::move(newPiece);
}

int Board::getWidth() const {
	return width;
}

int Board::getHeight() const {
	return height;
}
