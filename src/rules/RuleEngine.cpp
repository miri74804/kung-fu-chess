#include "RuleEngine.h"
#include "PieceRule.h"
#include "PieceRules.h"
#include "../model/Board.h"
#include "../model/Piece.h"

RuleEngine::RuleEngine() {
 	ruleRegistry[static_cast<int>(PieceType::NONE)] = nullptr;
	ruleRegistry[static_cast<int>(PieceType::KING)] = std::make_unique<KingRule>();
	ruleRegistry[static_cast<int>(PieceType::QUEEN)] = std::make_unique<QueenRule>();
	ruleRegistry[static_cast<int>(PieceType::ROOK)] = std::make_unique<RookRule>();
	ruleRegistry[static_cast<int>(PieceType::BISHOP)] = std::make_unique<BishopRule>();
	ruleRegistry[static_cast<int>(PieceType::KNIGHT)] = std::make_unique<KnightRule>();
	ruleRegistry[static_cast<int>(PieceType::PAWN)] = std::make_unique<PawnRule>();
}

MoveValidation RuleEngine::validateMove(const Board& board, const Position& from, const Position& to) const {
 	if (!board.isValidPosition(from) || !board.isValidPosition(to)) {
		return { false, "outside_board" };
	}

	Piece* piece = board.getPieceAt(from);
	if (piece == nullptr) {
		return { false, "empty_source" };
	}

	Piece* destinationPiece = board.getPieceAt(to);
	if (destinationPiece != nullptr && destinationPiece->getColor() == piece->getColor()) {
		return { false, "friendly_destination" };
	}

 	int ruleIndex = static_cast<int>(piece->getType());
	const auto& rule = ruleRegistry[ruleIndex];

 	if (!rule) {
		return { false, "illegal_piece_move" };
	}

 	if (!rule->isValidMove(from, to, board)) {
		return { false, "illegal_piece_move" };
	}

	return { true, "ok" };
}
