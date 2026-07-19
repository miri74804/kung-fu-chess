#include "GameState.h"

GameState::GameState() : gameOver(false), winner(Color::NONE) {}

bool GameState::isGameOver() const {
	return gameOver;
}

void GameState::setGameOver(bool value) {
	gameOver = value;
}

Color GameState::getWinner() const {
	return winner;
}

void GameState::setWinner(Color value) {
	winner = value;
}
