#include "PieceNotation.h"
#include "../utils/EnumLookup.h"
#include <utility>

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
	return lookupValue(PIECE_TABLE, type).value_or('.');
}

PieceType PieceNotation::parsePieceType(char c) {
	return lookupKey(PIECE_TABLE, c).value_or(PieceType::NONE);
}

char PieceNotation::toSymbol(Color color) {
	return lookupValue(COLOR_TABLE, color).value_or('.');
}

Color PieceNotation::parseColor(char c) {
	return lookupKey(COLOR_TABLE, c).value_or(Color::NONE);
}
