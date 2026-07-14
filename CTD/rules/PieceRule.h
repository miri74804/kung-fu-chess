#pragma once

#include "../Types.h"

class Board;
class Piece;

class PieceRule {
public:
	virtual ~PieceRule() = default;
	virtual bool isValidMove(const Position& from, const Position& to, const Board& board, const Piece* piece) const = 0;
};
