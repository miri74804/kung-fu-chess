#include "GameState.h"

GameState::GameState() : gameOver(false), winner(Color::NONE), whiteScore(0), blackScore(0) {}

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

void GameState::recordResignation(Color resigningColor) {
	gameOver = true;
	winner = resigningColor == Color::White ? Color::Black : Color::White;
}

void GameState::recordMove(const MoveLogEntry& entry) {
	moveLog.push_back(entry);
}

const std::vector<MoveLogEntry>& GameState::getMoveLog() const {
	return moveLog;
}

void GameState::addToScore(Color color, int value) {
	if (color == Color::White) {
		whiteScore += value;
	}
	else if (color == Color::Black) {
		blackScore += value;
	}
}

int GameState::getScore(Color color) const {
	return color == Color::White ? whiteScore : blackScore;
}
