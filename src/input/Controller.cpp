#include "Controller.h"
#include "BoardMapper.h"
#include "../model/Board.h"
#include "../model/Piece.h"
#include "../engine/GameEngine.h"

Controller::Controller() : isSelected(false), selectedPos(-1, -1) {}

void Controller::clearSelection() {
	isSelected = false;
	selectedPos = Position(-1, -1);
}

void Controller::click(int x, int y, GameEngine& gameEngine) {
	const Board& board = gameEngine.getBoard();

	Position pos;
	bool inBounds = BoardMapper::pixelToCell(x, y, board, pos);

	if (!inBounds) {
		if (isSelected) {
			clearSelection();
		}
		return;
	}

	if (!isSelected) {
		if (board.getPieceAt(pos) != nullptr) {
			isSelected = true;
			selectedPos = pos;
		}
		return;
	}

	Piece* clickedPiece = board.getPieceAt(pos);
	Piece* selectedPiece = board.getPieceAt(selectedPos);
	if (clickedPiece != nullptr && selectedPiece != nullptr && clickedPiece->getColor() == selectedPiece->getColor()) {
		selectedPos = pos;
		return;
	}

	gameEngine.requestMove(selectedPos, pos);
	clearSelection();
}
