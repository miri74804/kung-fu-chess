#include "RuleEngine.h"
#include "PieceRule.h"
#include "PieceRules.h"
#include "../model/Board.h"
#include "../model/Piece.h"

RuleEngine::RuleEngine() {
	ruleRegistry[static_cast<size_t>(PieceType::KING)] = std::make_unique<KingRule>();
	ruleRegistry[static_cast<size_t>(PieceType::QUEEN)] = std::make_unique<QueenRule>();
	ruleRegistry[static_cast<size_t>(PieceType::ROOK)] = std::make_unique<RookRule>();
	ruleRegistry[static_cast<size_t>(PieceType::BISHOP)] = std::make_unique<BishopRule>();
	ruleRegistry[static_cast<size_t>(PieceType::KNIGHT)] = std::make_unique<KnightRule>();
	ruleRegistry[static_cast<size_t>(PieceType::PAWN)] = std::make_unique<PawnRule>();
	// PieceType::NONE is left null - no real piece is ever created with
	// that type, so validateMove's null check below simply never triggers
	// for it in practice.
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

	const auto& rule = ruleRegistry[static_cast<size_t>(piece->getType())];
	if (!rule || !rule->isValidMove(from, to, board)) {
		return { false, "illegal_piece_move" };
	}

	return { true, "ok" };
}
