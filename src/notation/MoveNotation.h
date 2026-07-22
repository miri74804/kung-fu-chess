#pragma once

#include "../model/PieceType.h"
#include "../model/Position.h"
#include <string>

// Converts a completed move into standard algebraic chess notation
// (e.g. "e4", "Nc6", "Bxc6", "exd4"). Deliberately limited to what this
// project's rules actually support: no check ("+") or castling ("O-O"),
// since neither exists in RuleEngine, and no disambiguation between two
// identical pieces that could both reach the same square (e.g. "Rad1").
class MoveNotation {
public:
	static std::string generate(PieceType type, const Position& from, const Position& to,
		bool wasCapture, int boardHeight);
};
