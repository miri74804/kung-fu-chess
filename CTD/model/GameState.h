#pragma once

class Board;

class GameState {
private:
	Board& board;
	bool isGameOver;

public:
	GameState(Board& b);

	bool getIsGameOver() const;
	void setGameOver(bool value);
	const Board& getBoard() const;
};
