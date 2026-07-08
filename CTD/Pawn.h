#pragma once

#include "Piece.h"

class Pawn : public Piece {
public:
	Pawn(Color c) : Piece(c) {}
	char getSymbol() const override { return 'P'; }
	bool isValidMove(const Position& from, const Position& to, const Board& board) const override;
};
