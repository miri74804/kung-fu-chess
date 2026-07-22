#pragma once

#include "Color.h"
#include "MoveLogEntry.h"
#include <vector>

class GameState {
private:
	bool gameOver;
	Color winner;
	std::vector<MoveLogEntry> moveLog;
	int whiteScore;
	int blackScore;

public:
	GameState();

	bool isGameOver() const;
	Color getWinner() const;

	void recordKingCaptured(Color capturedKingColor);
	void recordMove(const MoveLogEntry& entry);
	const std::vector<MoveLogEntry>& getMoveLog() const;
	
	void addToScore(Color color, int value);
	int getScore(Color color) const;
};
