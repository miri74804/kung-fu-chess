#include "TextTestRunner.h"
#include <iostream>

void TextTestRunner::readCommands(Board& board) {
	GameEngine gameEngine(board);
	Controller controller;
	std::string line;

	while (std::getline(std::cin, line)) {
		trimCarriageReturn(line);

		if (!line.empty()) {
			ScriptCommand command = parser.parse(line);
			runCommand(command, gameEngine, controller);
		}
	}
}

void TextTestRunner::runCommand(const ScriptCommand& command, GameEngine& gameEngine, Controller& controller) {
	switch (command.type) {
	case CommandType::Click:
		controller.click(command.x, command.y, gameEngine);
		break;
	case CommandType::Jump: {
		Position pos;
		if (BoardMapper::pixelToCell(command.x, command.y, gameEngine.getBoard(), pos)) {
			gameEngine.handleJump(pos);
		}
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
