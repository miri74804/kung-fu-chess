#pragma once

#include "Types.h"
#include "model/Board.h"
#include "realtime/RealTimeArbiter.h"

class Piece;

class GameEngine {
private:
	Board& board;
	bool isSelected;
	Position selectedPos;
	bool isGameOver;
	RealTimeArbiter arbiter;

	void checkPawnPromotion(const Position& pos);

public:
	GameEngine(Board& b);

	void handleClick(const Position& pos);
	void handleJump(const Position& pos);
	void advanceTime(int ms);
	const Board& getBoard() const;
};
