#pragma once

enum class Color {
	NONE,
	White,
	Black
};

struct Position {
	int row;
	int col;

	Position() : row(-1), col(-1) {}
	Position(int r, int c) : row(r), col(c) {}
};

const char COLOR_SYMBOLS[] = {
	'.',  // NONE = 0
	'w',  // White = 1
	'b'   // Black = 2
};
