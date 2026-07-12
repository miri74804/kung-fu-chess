#pragma once

#include "Types.h"
#include "Board.h"

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

class GameState {
private:
	Board& board;
	bool isSelected;
	Position selectedPos;
	int gameClock;
	bool isMoveActive;
	MoveInProgress currentMove;
	bool isGameOver;
	bool isJumpActive;
	JumpInProgress currentJump;

	void checkPawnPromotion(const Position& pos);

public:
	GameState(Board& b);

	void handleClick(const Position& pos);
	void handleJump(const Position& pos);
	void advanceClock(int ms);
	void checkMovingPieces();
	void checkJumpingPieces();
	const Board& getBoard() const;
};
