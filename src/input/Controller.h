#pragma once

#include "../model/Position.h"
#include "../engine/GameSnapshot.h"

// What Controller::click concluded from a completed click-click pair: a
// legal-looking move the caller (Game) should send to the server. Whether
// it's actually legal is the server's call, not Controller's - the client
// no longer runs RuleEngine itself.
struct MoveRequest {
	bool requested;
	Position source;
	Position destination;
};

class Controller {
private:
	bool isSelected;
	Position selectedPos;

	void clearSelection();

public:
	Controller();

	MoveRequest click(int x, int y, const GameSnapshot& snapshot);

	// Read-only, for the Renderer to draw a selection highlight.
	bool hasSelection() const { return isSelected; }
	Position getSelectedPosition() const { return selectedPos; }
};
