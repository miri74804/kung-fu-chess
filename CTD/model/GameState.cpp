#include "GameState.h"
#include "Board.h"

GameState::GameState(Board& b) : board(b), isGameOver(false) {}

bool GameState::getIsGameOver() const {
	return isGameOver;
}

void GameState::setGameOver(bool value) {
	isGameOver = value;
}

const Board& GameState::getBoard() const {
	return board;
}
