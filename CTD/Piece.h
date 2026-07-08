#pragma once

#include "Types.h"

class Board;

class Piece {
protected:
	Color color;

	Piece(Color c) : color(c) {}

	bool isPathClear(const Position& from, const Position& to, const Board& board) const;

public:
	virtual ~Piece() = default;

	Color getColor() const { return color; }

	virtual char getSymbol() const = 0;  
	virtual bool isValidMove(const Position& from, const Position& to, const Board& board) const = 0;
};
