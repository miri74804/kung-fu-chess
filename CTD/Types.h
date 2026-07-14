#pragma once

#include <string>
#include "model/Position.h"

enum class Color {
	NONE,
	White,
	Black
};

enum class PieceType {
	NONE,
	KING,
	QUEEN,
	ROOK,
	BISHOP,
	KNIGHT,
	PAWN
};

const char COLOR_SYMBOLS[] = { '.', 'w', 'b' };

const char SYMBOLS[] = { '.', 'K', 'Q', 'R', 'B', 'N', 'P' };

struct MoveValidation {
	bool is_valid;
	std::string reason;
};

struct MoveResult {
	bool is_accepted;
	std::string reason;
};
