#include "CommandParser.h"
#include <iostream>
#include <sstream>

void CommandParser::readCommands(Board& board) {
	GameEngine gameEngine(board);
	std::string line;

	while (std::getline(std::cin, line)) {
		trimCarriageReturn(line);

		if (!line.empty()) {
			processCommand(line, gameEngine);
		}
	}
}

void CommandParser::processCommand(const std::string& line, GameEngine& gameEngine) {
	std::vector<std::string> parts = splitLine(line);

	if (parts.empty()) {
		return;
	}

	std::string command = parts[0];

	if (command == "click" && parts.size() == 3) {
		gameEngine.advanceTime(0);  // Sync motion before processing click

		int pixelX = std::stoi(parts[1]);
		int pixelY = std::stoi(parts[2]);

		int col = pixelX / 100;
		int row = pixelY / 100;
		Position pos(row, col);

		gameEngine.handleClick(pos);
	}
	else if (command == "jump" && parts.size() == 3) {
		gameEngine.advanceTime(0);  // Sync motion before processing jump

		int pixelX = std::stoi(parts[1]);
		int pixelY = std::stoi(parts[2]);

		int col = pixelX / 100;
		int row = pixelY / 100;
		Position pos(row, col);

		gameEngine.handleJump(pos);
	}
	else if (command == "wait" && parts.size() == 2) {
		int ms = std::stoi(parts[1]);
		gameEngine.advanceTime(ms);
	}
	else if (command == "print" && parts.size() == 2 && parts[1] == "board") {
		gameEngine.advanceTime(0);  // Sync motion before printing board
		BoardPrinter::print(gameEngine.getBoard());
	}
}
