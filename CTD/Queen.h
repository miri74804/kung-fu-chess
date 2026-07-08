#pragma once

#include "Piece.h"

class Queen : public Piece {
public:
	Queen(Color c) : Piece(c) {}
	char getSymbol() const override { return 'Q'; }
	bool isValidMove(const Position& from, const Position& to, const Board& board) const override;
};
