#pragma once

#include "motion.h"
#include "../model/Color.h"

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
	Color capturedKingColor;
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
	// Returns true (once - resets after reading) if a king was captured
	// since the last check, and fills outCapturedColor with that king's
	// color, so the caller can declare the OTHER color the winner.
	bool consumeKingWasCaptured(Color& outCapturedColor);
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
