#pragma once

#include "../model/Position.h"

class Piece;

struct MoveInProgress {
	Piece* movingPiece = nullptr;
	Position sourcePos;
	Position destinationPos;
	int startTime = 0;
	int arrivalTime = 0;
};

struct JumpInProgress {
	Piece* jumpingPiece = nullptr;
	Position cell;
	int landTime = 0;
};

// Everything a caller (GameEngine) needs to interpolate a piece's drawn
// position while it's mid-move. has=false (and the rest default/empty)
// when no move is active.
struct ActiveMoveInfo {
	bool has = false;
	Piece* piece = nullptr;
	Position source;
	Position destination;
	double progress = 0.0;
};
