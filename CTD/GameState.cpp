#include "GameState.h"
#include "Board.h"
#include "Piece.h"
#include "Types.h"

GameState::GameState(Board& b) : board(b), isSelected(false), selectedPos(-1, -1), gameClock(0), isMoveActive(false), isGameOver(false) {}

void GameState::checkPawnPromotion(const Position& pos) {
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
			board.promoteToQueen(pos);
		}
	}
}

void GameState::checkMovingPieces() {
	if (isMoveActive && gameClock >= currentMove.arrivalTime) {
		Piece* capturedPiece = board.getPieceAt(currentMove.destinationPos);
		if (capturedPiece != nullptr && capturedPiece->getSymbol() == 'K') {
			isGameOver = true;
		}

		board.movePieceOnBoard(currentMove.sourcePos, currentMove.destinationPos);
		checkPawnPromotion(currentMove.destinationPos);
		isMoveActive = false;
	}
}

void GameState::handleClick(const Position& pos) {
	if (isGameOver) {
		return;
	}

	Piece* clickedPiece = board.getPieceAt(pos);

	if (!isSelected) {
		if (clickedPiece != nullptr && !isMoveActive) {
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

	bool moveSuccessful = board.validateMove(selectedPos, pos);

	if (moveSuccessful) {
		if (selectedPieceToken != nullptr) {
			int duration = selectedPieceToken->calculateDuration(selectedPos, pos);

			currentMove.movingPiece = selectedPieceToken;
			currentMove.sourcePos = selectedPos;
			currentMove.destinationPos = pos;
			currentMove.arrivalTime = gameClock + duration;
			isMoveActive = true;

			isSelected = false;
			selectedPos = Position(-1, -1);
		}
	}
}

void GameState::advanceClock(int ms) {
	gameClock += ms;
}

const Board& GameState::getBoard() const {
	return board;
}