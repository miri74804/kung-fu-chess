#include "GameEngine.h"
#include "../model/Board.h"
#include "../model/Piece.h"
#include "../model/Color.h"
#include "../model/PieceType.h"
#include "../rules/RuleEngine.h"
#include "../rules/MoveGeometry.h"

GameEngine::GameEngine(Board& b) : board(b), gameState(), arbiter() {}

void GameEngine::checkPawnPromotion(const Position& pos) {
	Piece* piece = board.getPieceAt(pos);
	if (piece == nullptr) {
		return;
	}

	if (piece->getType() == PieceType::PAWN) {
		bool shouldPromote = false;

		if (piece->getColor() == Color::White && pos.row == 0) {
			shouldPromote = true;
		}
		else if (piece->getColor() == Color::Black && pos.row == board.getHeight() - 1) {
			shouldPromote = true;
		}

		if (shouldPromote) {
			auto newQueen = std::make_unique<Piece>(piece->getColor(), PieceType::QUEEN);
			board.setPieceAt(pos, std::move(newQueen));
		}
	}
}

MoveResult GameEngine::requestMove(const Position& source, const Position& destination) {
	if (gameState.isGameOver()) {
		return { false, "game_over" };
	}

	if (arbiter.hasActiveMotion()) {
		return { false, "motion_in_progress" };
	}

	RuleEngine ruleEngine;
	MoveValidation validation = ruleEngine.validateMove(board, source, destination);

	if (!validation.is_valid) {
		return { false, validation.reason };
	}

	Piece* movingPiece = board.getPieceAt(source);
	int duration = MoveGeometry::calculateDuration(source, destination);
	arbiter.startMotion(movingPiece, source, destination, duration);

	return { true, "ok" };
}

void GameEngine::handleJump(const Position& pos) {
	if (gameState.isGameOver()) {
		return;
	}

	Piece* piece = board.getPieceAt(pos);
	if (piece == nullptr) {
		return;
	}

	// Cannot jump while already moving or already jumping
	if (arbiter.hasActiveMotion() || arbiter.hasActiveJump()) {
		return;
	}

	arbiter.startJump(piece, pos, 1000);
}

void GameEngine::advanceTime(int ms) {
	arbiter.advanceTime(ms, board);

	// Check if a king was captured during motion
	if (arbiter.consumeKingWasCaptured()) {
		gameState.setGameOver(true);
	}

	// Check for pawn promotion at the destination of the last completed move
	Position lastDest = arbiter.getLastMoveDestination();
	if (lastDest.row >= 0 && lastDest.col >= 0) {
		checkPawnPromotion(lastDest);
		arbiter.resetLastMoveDestination();
	}
}

const Board& GameEngine::getBoard() const {
	return board;
}
