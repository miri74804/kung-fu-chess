#pragma once

#include "../Types.h"

class Board;
class Piece;

struct MoveInProgress {
	Piece* movingPiece = nullptr;
	Position sourcePos;
	Position destinationPos;
	int arrivalTime = 0;
};

struct JumpInProgress {
	Piece* jumpingPiece = nullptr;
	Position cell;
	int landTime = 0;
};

class RealTimeArbiter {
private:
	int gameClock;
	bool isMoveActive;
	MoveInProgress currentMove;
	bool isJumpActive;
	JumpInProgress currentJump;
	bool kingWasCaptured;
	Position lastMoveDestination;

	void processMoveArrivals(const Board& board);
	void processJumpLandings(const Board& board);

public:
	RealTimeArbiter();

	bool hasActiveMotion() const;
	bool hasActiveJump() const;
	void startMotion(Piece* piece, Position from, Position to, int duration);
	void startJump(Piece* piece, Position cell, int duration);
	void advanceTime(int ms, const Board& board);
	bool consumeKingWasCaptured();
	Position getLastMoveDestination() const;
	void resetLastMoveDestination();
	int getGameClock() const;
};
