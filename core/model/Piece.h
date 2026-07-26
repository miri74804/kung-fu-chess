#pragma once

#include "Color.h"
#include "PieceType.h"

class Piece {
private:
	Color color;
	PieceType type;

public:
	Piece(Color c, PieceType t);

	Color getColor() const { return color; }
	PieceType getType() const { return type; }
};
