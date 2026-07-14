#include "Piece.h"

Piece::Piece(Color c, PieceType t) 
	: color(c), type(t), movementSpeed(1000), isFirstMove(true) {}

char Piece::getSymbol() const {
	return PIECE_SYMBOLS[static_cast<int>(type)];
}
