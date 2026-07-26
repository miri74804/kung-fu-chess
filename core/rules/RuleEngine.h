#pragma once

#include "../model/Position.h"
#include "../model/PieceType.h"
#include "PieceRule.h"
#include <array>
#include <memory>
#include <string>

struct MoveValidation {
	bool isValid;
	std::string reason;
};

class Board;
class Piece;

class RuleEngine {
private:
	std::array<std::unique_ptr<PieceRule>, static_cast<size_t>(PieceType::COUNT)> ruleRegistry;

public:
	RuleEngine();

	MoveValidation validateMove(const Board& board, const Position& from, const Position& to) const;
};
