#pragma once

#include "../model/Position.h"

class Board;

class PieceRule {
public:
	virtual ~PieceRule() = default;
	virtual bool isValidMove(const Position& from, const Position& to, const Board& board) const = 0;
};
