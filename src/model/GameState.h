#pragma once

#include "Color.h"

class GameState {
private:
	bool gameOver;
	Color winner;

public:
	GameState();

	bool isGameOver() const;
	void setGameOver(bool value);

	Color getWinner() const;
	void setWinner(Color value);
};
