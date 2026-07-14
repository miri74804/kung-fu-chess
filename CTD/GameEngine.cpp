#include "GameEngine.h"
#include "model/Board.h"
#include "model/Piece.h"
#include "Types.h"
#include "rules/RuleEngine.h"

GameEngine::GameEngine(Board& b) : board(b), isSelected(false), selectedPos(-1, -1), isGameOver(false), arbiter() {}

void GameEngine::checkPawnPromotion(const Position& pos) {
	Piece* piece = board.getPieceAt(pos);
	if (piece == nullptr) {
		return;
	}

	if (piece->getSymbol() == 'P') {
		bool shouldPromote = false;

		if (piece->getColor() == Color::White && pos.row == 0) {
			shouldPromote = true;
		}
		else if (piece->getColor() == Color::Black && pos.row == board.getHeight() - 1) {
			shouldPromote = true;
		}

		if (shouldPromote) {
			// Create a new Queen piece with the pawn's color
			auto newQueen = std::make_unique<Piece>(piece->getColor(), PieceType::QUEEN);
			board.setPieceAt(pos, std::move(newQueen));
		}
	}
}

void GameEngine::handleClick(const Position& pos) {
	if (isGameOver) {
		return;
	}

	Piece* clickedPiece = board.getPieceAt(pos);

	if (!isSelected) {
		if (clickedPiece != nullptr && !arbiter.hasActiveMotion()) {
			isSelected = true;
			selectedPos = pos;
		}
		return;
	}

	Piece* selectedPieceToken = board.getPieceAt(selectedPos);

	if (clickedPiece != nullptr && selectedPieceToken != nullptr && clickedPiece->getColor() == selectedPieceToken->getColor()) {
		selectedPos = pos;
		return;
	}

	RuleEngine ruleEngine;
	MoveValidation validation = ruleEngine.validateMove(board, selectedPos, pos);

	if (validation.is_valid) {
		if (selectedPieceToken != nullptr) {
			int duration = selectedPieceToken->calculateDuration(selectedPos, pos);
			arbiter.startMotion(selectedPieceToken, selectedPos, pos, duration);

			isSelected = false;
			selectedPos = Position(-1, -1);
		}
	}
}

void GameEngine::handleJump(const Position& pos) {
	if (isGameOver) {
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
		isGameOver = true;
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
