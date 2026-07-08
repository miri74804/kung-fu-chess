#pragma once

#include "Piece.h"

class Knight : public Piece {
public:
	Knight(Color c) : Piece(c) {}
	char getSymbol() const override { return 'N'; }
	bool isValidMove(const Position& from, const Position& to, const Board& board) const override { return true; }
};
