#pragma once

enum class PieceType {
	NONE,
	KING,
	QUEEN,
	ROOK,
	BISHOP,
	KNIGHT,
	PAWN,
	COUNT // not a real piece type - always last, so its integer value equals the number of real entries above
};
