#pragma once

#include "Types.h"

class Board;
  
class Piece {
protected:
	Color color;
	int movementSpeed;
	bool isFirstMove;

	Piece(Color c) : color(c), movementSpeed(1000) {}

	bool isPathClear(const Position& from, const Position& to, const Board& board) const;
	bool isBlockedBySameColor(const Position& to, const Board& board) const;

public:
	virtual ~Piece() = default;

	Color getColor() const { return color; }

	virtual char getSymbol() const = 0;  
	virtual bool isValidMove(const Position& from, const Position& to, const Board& board) const = 0;
	virtual int calculateDuration(const Position& source, const Position& target) const;
};
