#include "TextTestRunner.h"
#include <iostream>

void TextTestRunner::readCommands(Board& board) {
	GameEngine gameEngine(board);
	std::string line;

	while (std::getline(std::cin, line)) {
		trimCarriageReturn(line);

		if (!line.empty()) {
			ScriptCommand command = parser.parse(line);
			runCommand(command, gameEngine);
		}
	}
}

void TextTestRunner::runCommand(const ScriptCommand& command, GameEngine& gameEngine) {
	// Pixel-to-cell mapping stays here (not in ScriptParser) until a dedicated
	// BoardMapper exists in input/ - it is coordinate mapping, not text parsing.
	switch (command.type) {
	case CommandType::Click: {
		int col = command.x / 100;
		int row = command.y / 100;
		gameEngine.handleClick(Position(row, col));
		break;
	}
	case CommandType::Jump: {
		int col = command.x / 100;
		int row = command.y / 100;
		gameEngine.handleJump(Position(row, col));
		break;
	}
	case CommandType::Wait:
		gameEngine.advanceTime(command.ms);
		break;
	case CommandType::PrintBoard:
		BoardPrinter::print(gameEngine.getBoard());
		break;
	case CommandType::Unknown:
	default:
		break;
	}
}
