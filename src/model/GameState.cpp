#include "GameState.h"

GameState::GameState() : gameOver(false), winner(Color::NONE) {}

bool GameState::isGameOver() const {
	return gameOver;
}

Color GameState::getWinner() const {
	return winner;
}

void GameState::recordKingCaptured(Color capturedKingColor) {
	gameOver = true;
	winner = capturedKingColor == Color::White ? Color::Black : Color::White;
}
