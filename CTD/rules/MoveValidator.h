#pragma once

#include "../Types.h"

class Board;
class Piece;

class MoveValidator {
public:
	static bool isPathClear(const Position& from, const Position& to, const Board& board);
	static bool isBlockedBySameColor(const Position& to, const Board& board, const Piece* piece);
	static int calculateDuration(const Position& source, const Position& target);
};
