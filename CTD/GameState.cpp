#include "GameState.h"
#include "Board.h"
#include "Types.h"

GameState::GameState(Board& b) : board(b), isSelected(false), selectedPos(-1, -1) {}

void GameState::handleClick(const Position& pos) {
	// Check if click is outside board
	if (!board.isValidPosition(pos)) {
		return;
	}

	Piece* clickedPiece = board.getPieceAt(pos);

	// If no piece selected yet
	if (!isSelected) {
		// If clicked on a piece, select it
		if (clickedPiece != nullptr) {
			isSelected = true;
			selectedPos = pos;
		}
		// Clicked on empty cell - ignore
		return;
	}

	// A piece is already selected
	Piece* selectedPieceToken = board.getPieceAt(selectedPos);

	// Check if clicking on another friendly piece (same color)
	if (clickedPiece != nullptr && selectedPieceToken != nullptr && clickedPiece->getColor() == selectedPieceToken->getColor()) {		// Replace selection
		selectedPos = pos;
		return;
	}

	// Otherwise, try to move the selected piece to the clicked cell
	bool moveSuccessful = board.movePiece(selectedPos, pos);

	// Only clear selection if move was successful
	if (moveSuccessful) {
		isSelected = false;
		selectedPos = Position(-1, -1);
	}
}

const Board& GameState::getBoard() const {
	return board;
}