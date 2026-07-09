#pragma once

#include "Types.h"

class Board;

class Piece {
protected:
	Color color;
	bool isMoving;
	int movementSpeed;

	Piece(Color c) : color(c), isMoving(false), movementSpeed(1000) {}

	bool isPathClear(const Position& from, const Position& to, const Board& board) const;
	bool isBlockedBySameColor(const Position& to, const Board& board) const;

public:
	virtual ~Piece() = default;

	Color getColor() const { return color; }
	bool getIsMoving() const { return isMoving; }
	void setMoving(bool moving) { isMoving = moving; }

	virtual char getSymbol() const = 0;  
	virtual bool isValidMove(const Position& from, const Position& to, const Board& board) const = 0;
	virtual int calculateDuration(const Position& source, const Position& target) const;
};
