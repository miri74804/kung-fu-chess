#pragma once

#include "../Types.h"
#include "../model/GameState.h"
#include "../realtime/RealTimeArbiter.h"

class Board;
class Piece;

class GameEngine {
private:
	Board& board;
	GameState gameState;
	bool isSelected;
	Position selectedPos;
	RealTimeArbiter arbiter;

	void checkPawnPromotion(const Position& pos);

public:
	GameEngine(Board& b);

	void handleClick(const Position& pos);
	void handleJump(const Position& pos);
	void advanceTime(int ms);
	const Board& getBoard() const;
};
