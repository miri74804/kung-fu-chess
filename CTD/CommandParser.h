#pragma once

#include <vector>
#include <string>
#include "model/Board.h"
#include "GameEngine.h"
#include "io/BoardPrinter.h"
#include "StringUtils.h"

class CommandParser {
public:
	void readCommands(Board& board);

private:
	void processCommand(const std::string& line, GameEngine& gameEngine);
};
