#pragma once

#include "../Types.h"

class Board;

class Piece {
private:
	Color color;
	PieceType type;
	int movementSpeed;
	bool isFirstMove;

public:
	Piece(Color c, PieceType t);
	~Piece() = default;

	Color getColor() const { return color; }
	PieceType getType() const { return type; }
	char getSymbol() const;
	int calculateDuration(const Position& source, const Position& target) const;
	bool getIsFirstMove() const { return isFirstMove; }
	void setIsFirstMove(bool value) { isFirstMove = value; }

	bool isPathClear(const Position& from, const Position& to, const Board& board) const;
	bool isBlockedBySameColor(const Position& to, const Board& board) const;
};
