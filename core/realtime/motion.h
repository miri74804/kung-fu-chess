#pragma once

#include "../model/Color.h"
#include "../model/PieceType.h"
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

// A move that just landed this tick, with everything needed to log it -
// including whether it captured a piece, which is only known on arrival
// (the defender could have moved away mid-flight). has=false when no move
// landed this tick.
struct CompletedMoveInfo {
	bool has = false;
	Color color = Color::NONE;
	PieceType type = PieceType::NONE;
	Position source;
	Position destination;
	bool wasCapture = false;
	PieceType capturedType = PieceType::NONE;
	int gameClockMs = 0;
};
