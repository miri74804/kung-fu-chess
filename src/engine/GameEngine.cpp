#include "GameEngine.h"
#include "../model/Board.h"
#include "../model/Piece.h"
#include "../model/Color.h"
#include "../model/PieceType.h"
#include "../rules/RuleEngine.h"
#include "../rules/MoveGeometry.h"

namespace {
	// Cells the move from source to destination passes through (source
	// excluded, destination included). Only meaningful for a straight or
	// diagonal line (sliding pieces); a knight-like jump has no real path,
	// so it's just the destination.
	std::vector<Position> computeMovePath(const Position& source, const Position& destination) {
		std::vector<Position> path;

		if (!MoveGeometry::isStraightLine(source, destination) && !MoveGeometry::isDiagonal(source, destination)) {
			path.push_back(destination);
			return path;
		}

		int rowDiff = destination.row - source.row;
		int colDiff = destination.col - source.col;
		int rowStep = (rowDiff == 0) ? 0 : (rowDiff > 0 ? 1 : -1);
		int colStep = (colDiff == 0) ? 0 : (colDiff > 0 ? 1 : -1);

		Position current(source.row + rowStep, source.col + colStep);
		while (true) {
			path.push_back(current);
			if (current.row == destination.row && current.col == destination.col) {
				break;
			}
			current = Position(current.row + rowStep, current.col + colStep);
		}
		return path;
	}
}

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
	Color capturedKingColor;
	if (arbiter.consumeKingWasCaptured(capturedKingColor)) {
		gameState.setGameOver(true);
		gameState.setWinner(capturedKingColor == Color::White ? Color::Black : Color::White);
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

bool GameEngine::isMotionInProgress() const {
	return arbiter.hasActiveMotion();
}

bool GameEngine::isGameOver() const {
	return gameState.isGameOver();
}

GameSnapshot GameEngine::snapshot() const {
	GameSnapshot snap;
	snap.boardWidth = board.getWidth();
	snap.boardHeight = board.getHeight();
	snap.gameOver = gameState.isGameOver();
	snap.winner = gameState.getWinner();

	bool hasActive = arbiter.hasActiveMotion();
	Piece* activePiece = hasActive ? arbiter.getActiveMovePiece() : nullptr;
	Position activeSource = hasActive ? arbiter.getActiveMoveSource() : Position();
	Position activeDest = hasActive ? arbiter.getActiveMoveDestination() : Position();
	double progress = hasActive ? arbiter.getActiveMoveProgress() : 0.0;

	for (int row = 0; row < board.getHeight(); ++row) {
		for (int col = 0; col < board.getWidth(); ++col) {
			Piece* piece = board.getPieceAt(Position(row, col));
			if (piece == nullptr) {
				continue;
			}

			PieceSnapshot pieceSnap;
			pieceSnap.color = piece->getColor();
			pieceSnap.type = piece->getType();

			if (hasActive && piece == activePiece) {
				pieceSnap.row = activeSource.row + (activeDest.row - activeSource.row) * progress;
				pieceSnap.col = activeSource.col + (activeDest.col - activeSource.col) * progress;
			}
			else {
				pieceSnap.row = row;
				pieceSnap.col = col;
			}

			snap.pieces.push_back(pieceSnap);
		}
	}

	if (hasActive) {
		snap.activeMovePath = computeMovePath(activeSource, activeDest);
	}

	return snap;
}
