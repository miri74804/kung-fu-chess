#pragma once

#include "PieceRule.h"

class RookRule : public PieceRule {
public:
	bool isValidMove(const Position& from, const Position& to, const Board& board, const Piece* piece) const override;
};
