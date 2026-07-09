#pragma once

#include "Types.h"
#include "Board.h"
#include <vector>

class Piece;

struct MoveInProgress {
	Piece* movingPiece;
	Position sourcePos;
	Position destinationPos;
	int arrivalTime;
};

class GameState {
private:
	Board& board;
	bool isSelected;
	Position selectedPos;
	int gameClock;
	std::vector<MoveInProgress> activeMoves;

public:
	GameState(Board& b);

	void handleClick(const Position& pos);
	void advanceClock(int ms);
	void checkMovingPieces();
	const Board& getBoard() const;
};
