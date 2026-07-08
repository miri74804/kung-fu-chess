#pragma once

#include "Types.h"
#include "Board.h"

class GameState {
private:
	Board& board;
	bool isSelected;
	Position selectedPos;

public:
	GameState(Board& b);

	void handleClick(const Position& pos);
	const Board& getBoard() const;
};
