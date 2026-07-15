#include "Piece.h"

Piece::Piece(Color c, PieceType t)
	: color(c), type(t), movementSpeed(1000), isFirstMove(true) {}
