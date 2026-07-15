#include "GameState.h"

GameState::GameState() : gameOver(false) {}

bool GameState::isGameOver() const {
	return gameOver;
}

void GameState::setGameOver(bool value) {
	gameOver = value;
}
