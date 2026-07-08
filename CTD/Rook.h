#pragma once

#include "Piece.h"

class Rook : public Piece {
public:
	Rook(Color c) : Piece(c) {}
	char getSymbol() const override { return 'R'; }
	bool isValidMove(const Position& from, const Position& to, const Board& board) const override;
};
