#pragma once

#include "Color.h"

class GameState {
private:
	bool gameOver;
	Color winner;

public:
	GameState();

	bool isGameOver() const;
	Color getWinner() const;

	// The only way gameOver/winner ever change: bundles both into one
	// atomic step (the losing king's color in, gameOver=true and the
	// OTHER color as winner out), so the two fields can never be set
	// inconsistently or one set without the other.
	void recordKingCaptured(Color capturedKingColor);
};
