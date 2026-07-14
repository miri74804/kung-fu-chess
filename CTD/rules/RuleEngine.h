#pragma once

#include "../Types.h"
#include "PieceRule.h"
#include <memory>

class Board;
class Piece;

class RuleEngine {
private:
	std::unique_ptr<PieceRule> ruleRegistry[7];

public:
	RuleEngine();

	MoveValidation validateMove(const Board& board, const Position& from, const Position& to) const;
};
