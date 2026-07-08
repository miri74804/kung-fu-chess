#pragma once

#include <vector>
#include <string>
#include "Board.h"
#include "GameState.h"
#include "StringUtils.h"

class CommandParser {
public:
	void readCommands(Board& board);

private:
	void processCommand(const std::string& line, GameState& gameState);
};
