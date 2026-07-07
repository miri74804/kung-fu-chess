#include "Board.h"
#include <iostream>

Board::Board() : expectedWidth(0) {}

bool Board::addRow(const std::vector<Token>& row) {
	if (row.empty()) {
		return false;
	}

	if (board.empty()) {
		expectedWidth = row.size();
	} else if (row.size() != expectedWidth) {
			return false;
	}

	board.push_back(row);
	return true;
}

void Board::print() const {
	for (size_t i = 0; i < board.size(); ++i) {
		for (size_t j = 0; j < board[i].size(); ++j) {
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
