#pragma once

#include "../Types.h"

class Piece {
private:
	Color color;
	PieceType type;
	int movementSpeed;
	bool isFirstMove;

public:
	Piece(Color c, PieceType t);

	Color getColor() const { return color; }
	PieceType getType() const { return type; }
	char getSymbol() const;
	bool getIsFirstMove() const { return isFirstMove; }
	void setIsFirstMove(bool value) { isFirstMove = value; }
};
