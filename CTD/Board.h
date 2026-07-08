#pragma once

#include <vector>
#include <memory>
#include "Types.h"
#include "Piece.h"

class Board {
private:
	std::vector<std::vector<std::unique_ptr<Piece>>> board;
	int width;

public:
	Board();

	bool addRow(std::vector<std::unique_ptr<Piece>>& row);
	void print() const;

	bool isValidPosition(const Position& pos) const;	
	Piece* getPieceAt(const Position& pos) const;
	bool movePiece(const Position& from, const Position& to);
	int getWidth() const;
	int getHeight() const;
};
