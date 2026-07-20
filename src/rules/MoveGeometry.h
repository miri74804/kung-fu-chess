#pragma once

#include "../model/Position.h"
#include <vector>

class Board;
class Piece;

class MoveGeometry {
public:
	static bool isPathClear(const Position& from, const Position& to, const Board& board);
	static bool isBlockedBySameColor(const Position& to, const Board& board, const Piece* piece);
	static int calculateDuration(const Position& source, const Position& target);

	static bool isStraightLine(const Position& from, const Position& to);
	static bool isDiagonal(const Position& from, const Position& to);

	// Cells the move passes through, source excluded and destination included.
	static std::vector<Position> computePath(const Position& source, const Position& destination);
};
