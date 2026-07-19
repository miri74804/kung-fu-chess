#pragma once

#include "../model/Position.h"

class GameEngine;

class Controller {
private:
	bool isSelected;
	Position selectedPos;
	bool lastMoveWasRejected;
	Position lastRejectedPosition;

	void clearSelection();

public:
	Controller();

	void click(int x, int y, GameEngine& gameEngine);

	// Read-only, for the Renderer to draw a selection highlight.
	bool hasSelection() const { return isSelected; }
	Position getSelectedPosition() const { return selectedPos; }

	// Read-only, for the Renderer to flag the last rejected move attempt
	// (illegal destination, or a friendly piece there). Stays set until the
	// next click, whatever its outcome.
	bool wasLastMoveRejected() const { return lastMoveWasRejected; }
	Position getLastRejectedPosition() const { return lastRejectedPosition; }
};
