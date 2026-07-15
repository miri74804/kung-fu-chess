#pragma once

#include <string>
#include "../model/Board.h"
#include "../engine/GameEngine.h"
#include "../io/BoardPrinter.h"
#include "../utils/StringUtils.h"
#include "ScriptParser.h"

class TextTestRunner {
public:
	void readCommands(Board& board);

private:
	ScriptParser parser;

	void runCommand(const ScriptCommand& command, GameEngine& gameEngine);
};
