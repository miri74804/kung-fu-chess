#pragma once

class Board;
struct Position;

class BoardMapper {
public:
	static bool pixelToCell(int x, int y, const Board& board, Position& outPosition);
};
