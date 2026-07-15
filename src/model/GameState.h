#pragma once

class GameState {
private:
	bool gameOver;

public:
	GameState();

	bool isGameOver() const;
	void setGameOver(bool value);
};
