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

struct MoveValidation {
	bool is_valid;
	std::string reason;
};

struct MoveResult {
	bool is_accepted;
	std::string reason;
};
