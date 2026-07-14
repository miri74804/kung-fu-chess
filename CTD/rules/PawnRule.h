#pragma once

#include "PieceRule.h"

class PawnRule : public PieceRule {
public:
	bool isValidMove(const Position& from, const Position& to, const Board& board, const Piece* piece) const override;
};
