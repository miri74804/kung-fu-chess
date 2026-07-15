#pragma once

#include "../model/Position.h"

class GameEngine;

class Controller {
private:
	bool isSelected;
	Position selectedPos;

	void clearSelection();

public:
	Controller();

	void click(int x, int y, GameEngine& gameEngine);
};
