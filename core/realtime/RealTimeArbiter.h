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
	CompletedMoveInfo completedMove;

	void processMoveArrivals(Board& board);
	void processJumpLandings(Board& board);

	// The two outcomes processMoveArrivals can resolve once a move's
	// arrival time is reached - split out since they mutate different
	// state and only one of them ever runs per arrival.
	void resolveAirborneDefenseCapture(Board& board);
	void resolveNormalArrival(Board& board, Piece* defenderAtDestination);

public:
	RealTimeArbiter();

	bool hasActiveMotion() const;
	bool hasActiveJump() const;
	void startMotion(Piece* piece, Position from, Position to, int duration);
	void startJump(Piece* piece, Position cell, int duration);
	void advanceTime(int ms, Board& board);
	bool consumeKingWasCaptured(Color& outCapturedColor);
	Position getLastMoveDestination() const;
	void resetLastMoveDestination();

	// True (once) the tick a move lands normally - false for a move that
	// never completes (e.g. the mover got captured mid-flight via airborne
	// defense). The caller (GameEngine) uses this to log the move and, if
	// wasCapture, credit the mover's score.
	bool consumeCompletedMove(CompletedMoveInfo& outInfo);

	// Everything needed to interpolate the in-flight piece's drawn position
	// (has=false, with the rest default, when no move is active) - one call
	// instead of hasActiveMotion() plus four separate getters.
	ActiveMoveInfo getActiveMoveInfo() const;
};
