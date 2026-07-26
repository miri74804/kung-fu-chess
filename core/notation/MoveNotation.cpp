#include "MoveNotation.h"
#include "PieceNotation.h"

namespace {
	std::string squareName(const Position& pos, int boardHeight) {
		char file = static_cast<char>('a' + pos.col);
		int rank = boardHeight - pos.row;
		return std::string(1, file) + std::to_string(rank);
	}
}

std::string MoveNotation::generate(PieceType type, const Position& from, const Position& to,
	bool wasCapture, int boardHeight) {
	std::string notation;

	if (type != PieceType::PAWN) {
		notation += PieceNotation::toSymbol(type);
	}
	else if (wasCapture) {
		// A pawn capture is prefixed with its source file (e.g. "exd4"),
		// since a pawn has no letter of its own in chess notation - unlike
		// PieceNotation::toSymbol(PAWN), which returns 'P' for the io text
		// board format, where every square needs some character.
		notation += static_cast<char>('a' + from.col);
	}

	if (wasCapture) {
		notation += 'x';
	}

	notation += squareName(to, boardHeight);
	return notation;
}
