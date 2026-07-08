#pragma once

#include "Piece.h"

class Bishop : public Piece {
public:
	Bishop(Color c) : Piece(c) {}
	char getSymbol() const override { return 'B'; }
	bool isValidMove(const Position& from, const Position& to, const Board& board) const override { return true; }
};
