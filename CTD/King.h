#pragma once

#include "Piece.h"

class King : public Piece {
public:
	King(Color c) : Piece(c) {}
	char getSymbol() const override { return 'K'; }
	bool isValidMove(const Position& from, const Position& to, const Board& board) const override;
};
