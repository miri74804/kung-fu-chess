#pragma once

#include "../model/Position.h"
#include "../model/GameState.h"
#include "../realtime/RealTimeArbiter.h"
#include "GameSnapshot.h"
#include <string>
#include <vector>

struct MoveResult {
	bool isAccepted;
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
	std::vector<PieceSnapshot> buildPieceSnapshots(const ActiveMoveInfo& activeMove) const;

public:
	GameEngine(Board& b);

	MoveResult requestMove(const Position& source, const Position& destination);
	void handleJump(const Position& pos);
	void advanceTime(int ms);
	const Board& getBoard() const;
	GameSnapshot snapshot() const;

	// Read-only query for callers (e.g. an input loop) that want to skip
	// dispatching a click at all while a motion is active - Controller
	// itself must stay blind to timing, per the architecture.
	bool isMotionInProgress() const;

	// Same idea: once the game is over, an input loop should stop
	// dispatching clicks entirely (no more selecting/moving pieces).
	bool isGameOver() const;
};
