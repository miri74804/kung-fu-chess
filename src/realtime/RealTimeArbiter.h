#pragma once

#include "motion.h"

class Board;
class Piece;

class RealTimeArbiter {
private:
	int gameClock;
	bool isMoveActive;
	MoveInProgress currentMove;
	bool isJumpActive;
	JumpInProgress currentJump;
	bool kingWasCaptured;
	Position lastMoveDestination;

	void processMoveArrivals(Board& board);
	void processJumpLandings(Board& board);

public:
	RealTimeArbiter();

	bool hasActiveMotion() const;
	bool hasActiveJump() const;
	void startMotion(Piece* piece, Position from, Position to, int duration);
	void startJump(Piece* piece, Position cell, int duration);
	void advanceTime(int ms, Board& board);
	bool consumeKingWasCaptured();
	Position getLastMoveDestination() const;
	void resetLastMoveDestination();
	int getGameClock() const;

	// For interpolated rendering while a move is in flight (hasActiveMotion()
	// must be true). progress is 0.0 at the start of the move and 1.0 right
	// before arrival.
	Piece* getActiveMovePiece() const;
	Position getActiveMoveSource() const;
	Position getActiveMoveDestination() const;
	double getActiveMoveProgress() const;
};
