#pragma once

#include "../model/Position.h"

class Board;
class Piece;

class MoveGeometry {
public:
	static bool isPathClear(const Position& from, const Position& to, const Board& board);
	static bool isBlockedBySameColor(const Position& to, const Board& board, const Piece* piece);
	static int calculateDuration(const Position& source, const Position& target);

	static bool isStraightLine(const Position& from, const Position& to);
	static bool isDiagonal(const Position& from, const Position& to);
};
