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

class GameState {
private:
	Board& board;
	bool isSelected;
	Position selectedPos;
	int gameClock;
	bool isMoveActive;
	MoveInProgress currentMove;
	bool isGameOver;

	void checkPawnPromotion(const Position& pos);

public:
	GameState(Board& b);

	void handleClick(const Position& pos);
	void advanceClock(int ms);
	void checkMovingPieces();
	const Board& getBoard() const;
};
