#pragma once

struct Position {
	int row;
	int col;

	Position() : row(-1), col(-1) {}
	Position(int r, int c) : row(r), col(c) {}
};
