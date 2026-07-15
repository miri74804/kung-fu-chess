#pragma once

#include <vector>
#include <string>
#include "../model/Board.h"
#include "../engine/GameEngine.h"
#include "../io/BoardPrinter.h"
#include "../utils/StringUtils.h"

class TextTestRunner {
public:
	void readCommands(Board& board);

private:
	void processCommand(const std::string& line, GameEngine& gameEngine);
};
