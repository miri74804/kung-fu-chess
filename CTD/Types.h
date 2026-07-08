#pragma once

enum class Piece {
	NONE,
	King,
	Queen,
	Rook,
	Knight,
	Pawn
};

enum class Color {
	NONE,
	White,
	Black
};

struct Token {
	Color color;
	Piece piece;

	bool isEmpty() const {
		return color == Color::NONE || piece == Piece::NONE;
	}
};

struct Position {
	int row;
	int col;

	Position() : row(-1), col(-1) {}
	Position(int r, int c) : row(r), col(c) {}
};

const char PIECE_SYMBOLS[] = {
	'.',  // NONE = 0
	'K',  // King = 1
	'Q',  // Queen = 2
	'R',  // Rook = 3
	'N',  // Knight = 4
	'P'   // Pawn = 5
};

const char COLOR_SYMBOLS[] = {
	'.',  // NONE = 0
	'w',  // White = 1
	'b'   // Black = 2
};
