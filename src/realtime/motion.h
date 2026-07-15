#pragma once

#include "../model/Position.h"

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
