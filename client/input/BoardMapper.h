#pragma once

struct Position;

class BoardMapper {
public:
	static bool pixelToCell(int x, int y, int boardWidth, int boardHeight, Position& outPosition);
};
