#include "Controller.h"
#include "BoardMapper.h"

namespace {
	// Selection/reselection only ever happens between clicks, and clicks are
	// only dispatched when no motion is active (Game::run), so every piece
	// in the snapshot sits exactly on a whole cell - no fractional match needed.
	const PieceSnapshot* findPieceAt(const GameSnapshot& snapshot, const Position& pos) {
		for (const PieceSnapshot& piece : snapshot.pieces) {
			if (static_cast<int>(piece.row) == pos.row && static_cast<int>(piece.col) == pos.col) {
				return &piece;
			}
		}
		return nullptr;
	}
}

Controller::Controller() : isSelected(false), selectedPos(-1, -1) {}

void Controller::clearSelection() {
	isSelected = false;
	selectedPos = Position(-1, -1);
}

MoveRequest Controller::click(int x, int y, const GameSnapshot& snapshot, Color myColor) {
	Position pos;
	bool inBounds = BoardMapper::pixelToCell(x, y, snapshot.boardWidth, snapshot.boardHeight, pos);

	if (!inBounds) {
		if (isSelected) {
			clearSelection();
		}
		return { false, Position(), Position() };
	}

	if (!isSelected) {
		const PieceSnapshot* piece = findPieceAt(snapshot, pos);
		if (piece != nullptr && piece->color == myColor) {
			isSelected = true;
			selectedPos = pos;
		}
		return { false, Position(), Position() };
	}

	const PieceSnapshot* clickedPiece = findPieceAt(snapshot, pos);
	const PieceSnapshot* selectedPiece = findPieceAt(snapshot, selectedPos);
	if (clickedPiece != nullptr && selectedPiece != nullptr && clickedPiece->color == selectedPiece->color) {
		selectedPos = pos;
		return { false, Position(), Position() };
	}

	MoveRequest request{ true, selectedPos, pos };
	clearSelection();
	return request;
}
