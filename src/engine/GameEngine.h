#pragma once

#include "../model/Position.h"
#include "../model/GameState.h"
#include "../realtime/RealTimeArbiter.h"
#include "GameSnapshot.h"
#include <string>

struct MoveResult {
	bool is_accepted;
	std::string reason;
};

class Board;
class Piece;

class GameEngine {
private:
	Board& board;
	GameState gameState;
	RealTimeArbiter arbiter;

	void checkPawnPromotion(const Position& pos);

public:
	GameEngine(Board& b);

	MoveResult requestMove(const Position& source, const Position& destination);
	void handleJump(const Position& pos);
	void advanceTime(int ms);
	const Board& getBoard() const;
	GameSnapshot snapshot() const;
};
