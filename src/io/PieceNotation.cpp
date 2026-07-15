#include "PieceNotation.h"
#include <utility>
#include <iterator>

namespace {
	const std::pair<PieceType, char> PIECE_TABLE[] = {
		{ PieceType::NONE,   '.' },
		{ PieceType::KING,   'K' },
		{ PieceType::QUEEN,  'Q' },
		{ PieceType::ROOK,   'R' },
		{ PieceType::BISHOP, 'B' },
		{ PieceType::KNIGHT, 'N' },
		{ PieceType::PAWN,   'P' },
	};

	const std::pair<Color, char> COLOR_TABLE[] = {
		{ Color::NONE,  '.' },
		{ Color::White, 'w' },
		{ Color::Black, 'b' },
	};
}

char PieceNotation::toSymbol(PieceType type) {
	for (int i = 0; i < std::size(PIECE_TABLE); ++i) {
		if (PIECE_TABLE[i].first == type) {
			return PIECE_TABLE[i].second;
		}
	}
	return '.';
}

PieceType PieceNotation::parsePieceType(char c) {
	for (int i = 0; i < std::size(PIECE_TABLE); ++i) {
		if (PIECE_TABLE[i].second == c) {
			return PIECE_TABLE[i].first;
		}
	}
	return PieceType::NONE;
}

char PieceNotation::toSymbol(Color color) {
	for (int i = 0; i < std::size(COLOR_TABLE); ++i) {
		if (COLOR_TABLE[i].first == color) {
			return COLOR_TABLE[i].second;
		}
	}
	return '.';
}

Color PieceNotation::parseColor(char c) {
	for (int i = 0; i < std::size(COLOR_TABLE); ++i) {
		if (COLOR_TABLE[i].second == c) {
			return COLOR_TABLE[i].first;
		}
	}
	return Color::NONE;
}
