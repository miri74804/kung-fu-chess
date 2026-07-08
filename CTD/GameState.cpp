#include "GameState.h"
#include "Board.h"
#include "Types.h"

GameState::GameState(Board& b) : board(b), isSelected(false), selectedPos(-1, -1), gameClock(0) {}

void GameState::handleClick(const Position& pos) {
	if (!board.isValidPosition(pos)) {
		return;
	}

	Piece* clickedPiece = board.getPieceAt(pos);

	if (!isSelected) {
		if (clickedPiece != nullptr) {
			isSelected = true;
			selectedPos = pos;
		}
		return;
	}

	Piece* selectedPieceToken = board.getPieceAt(selectedPos);

	if (clickedPiece != nullptr && selectedPieceToken != nullptr && clickedPiece->getColor() == selectedPieceToken->getColor()) {		// Replace selection
		selectedPos = pos;
		return;
	}

	bool moveSuccessful = board.movePiece(selectedPos, pos);

	if (moveSuccessful) {
		isSelected = false;
		selectedPos = Position(-1, -1);
	}
}

void GameState::advanceClock(int ms) {
	gameClock += ms;
}

const Board& GameState::getBoard() const {
	return board;
}