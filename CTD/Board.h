#pragma once

#include <vector>
#include "Types.h"

class Board {
private:
	std::vector<std::vector<Token>> board;
	int width;

public:
	Board();

	bool addRow(const std::vector<Token>& row);
	void print() const;

	bool isValidPosition(const Position& pos) const;	
	Token getPieceAt(const Position& pos) const;
	bool movePiece(const Position& from, const Position& to);
	int getWidth() const;
	int getHeight() const;
};
