#pragma once

#include "../model/Position.h"
#include "PieceRule.h"
#include <memory>
#include <string>

struct MoveValidation {
	bool is_valid;
	std::string reason;
};

class Board;
class Piece;

class RuleEngine {
private:
	std::unique_ptr<PieceRule> ruleRegistry[7];

public:
	RuleEngine();

	MoveValidation validateMove(const Board& board, const Position& from, const Position& to) const;
};
