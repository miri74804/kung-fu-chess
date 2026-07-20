#include "GameEngine.h"
#include "../model/Board.h"
#include "../model/Piece.h"
#include "../model/Color.h"
#include "../model/PieceType.h"
#include "../rules/RuleEngine.h"
#include "../rules/MoveGeometry.h"
#include "../Constants.h"

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

	if (!validation.isValid) {
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

	if (arbiter.hasActiveMotion() || arbiter.hasActiveJump()) {
		return;
	}

	arbiter.startJump(piece, pos, JUMP_DURATION_MS);
}

void GameEngine::advanceTime(int ms) {
	arbiter.advanceTime(ms, board);

	Color capturedKingColor;
	if (arbiter.consumeKingWasCaptured(capturedKingColor)) {
		gameState.recordKingCaptured(capturedKingColor);
	}

	Position lastDest = arbiter.getLastMoveDestination();
	if (lastDest.row >= 0 && lastDest.col >= 0) {
		checkPawnPromotion(lastDest);
		arbiter.resetLastMoveDestination();
	}
}

const Board& GameEngine::getBoard() const {
	return board;
}

bool GameEngine::isMotionInProgress() const {
	return arbiter.hasActiveMotion();
}

bool GameEngine::isGameOver() const {
	return gameState.isGameOver();
}

std::vector<PieceSnapshot> GameEngine::buildPieceSnapshots(const ActiveMoveInfo& activeMove) const {
	std::vector<PieceSnapshot> pieceSnapshots;

	for (int row = 0; row < board.getHeight(); ++row) {
		for (int col = 0; col < board.getWidth(); ++col) {
			Piece* piece = board.getPieceAt(Position(row, col));
			if (piece == nullptr) {
				continue;
			}

			PieceSnapshot pieceSnap;
			pieceSnap.color = piece->getColor();
			pieceSnap.type = piece->getType();

			if (activeMove.has && piece == activeMove.piece) {
				double rowDiff = activeMove.destination.row - activeMove.source.row;
				double colDiff = activeMove.destination.col - activeMove.source.col;
				pieceSnap.row = activeMove.source.row + rowDiff * activeMove.progress;
				pieceSnap.col = activeMove.source.col + colDiff * activeMove.progress;
			}
			else {
				pieceSnap.row = row;
				pieceSnap.col = col;
			}

			pieceSnapshots.push_back(pieceSnap);
		}
	}

	return pieceSnapshots;
}

GameSnapshot GameEngine::snapshot() const {
	GameSnapshot snap;
	snap.boardWidth = board.getWidth();
	snap.boardHeight = board.getHeight();
	snap.gameOver = gameState.isGameOver();
	snap.winner = gameState.getWinner();

	ActiveMoveInfo activeMove = arbiter.getActiveMoveInfo();
	snap.pieces = buildPieceSnapshots(activeMove);

	if (activeMove.has) {
		snap.activeMovePath = MoveGeometry::computePath(activeMove.source, activeMove.destination);
	}

	return snap;
}
