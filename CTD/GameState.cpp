#include "GameState.h"
#include "Board.h"
#include "Piece.h"
#include "Types.h"

GameState::GameState(Board& b) : board(b), isSelected(false), selectedPos(-1, -1), gameClock(0) {}

void GameState::checkMovingPieces() {
	for (int i = static_cast<int>(activeMoves.size()) - 1; i >= 0; --i) {
		if (gameClock >= activeMoves[i].arrivalTime) {
			board.movePieceOnBoard(activeMoves[i].sourcePos, activeMoves[i].destinationPos);
			activeMoves[i].movingPiece->setMoving(false);
			activeMoves.erase(activeMoves.begin() + i);
		}
	}
}

void GameState::handleClick(const Position& pos) {
	Piece* clickedPiece = board.getPieceAt(pos);

	if (!isSelected) {
		if (clickedPiece != nullptr && !clickedPiece->getIsMoving()) {
			isSelected = true;
			selectedPos = pos;
		}
		return;
	}

	Piece* selectedPieceToken = board.getPieceAt(selectedPos);

	if (clickedPiece != nullptr && !clickedPiece->getIsMoving() &&
		selectedPieceToken != nullptr && clickedPiece->getColor() == selectedPieceToken->getColor()) {
		selectedPos = pos;
		return;
	}

	bool moveSuccessful = board.validateMove(selectedPos, pos);

	if (moveSuccessful) {
		if (selectedPieceToken != nullptr) {
			int duration = selectedPieceToken->calculateDuration(selectedPos, pos);
			int arrivalTime = gameClock + duration;

			selectedPieceToken->setMoving(true);

			activeMoves.push_back({selectedPieceToken, selectedPos, pos, arrivalTime});

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