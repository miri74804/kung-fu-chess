#pragma once

#include "Types.h"
#include "Board.h"

class GameState {
private:
	Board& board;
	bool isSelected;
	Position selectedPos;
	int gameClock;

public:
	GameState(Board& b);

	void handleClick(const Position& pos);
	void advanceClock(int ms);
	const Board& getBoard() const;
};
