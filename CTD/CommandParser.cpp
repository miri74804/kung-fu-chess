#include "CommandParser.h"
#include <iostream>
#include <sstream>

void CommandParser::readCommands(Board& board) {
	GameState gameState(board);
	std::string line;

	while (std::getline(std::cin, line)) {
		trimCarriageReturn(line);

		if (!line.empty()) {
			processCommand(line, gameState);
		}
	}
}

void CommandParser::processCommand(const std::string& line, GameState& gameState) {
	std::vector<std::string> parts = splitLine(line);

	if (parts.empty()) {
		return;
	}

	std::string command = parts[0];

	if (command == "click" && parts.size() == 3) {
		int pixelX = std::stoi(parts[1]);
		int pixelY = std::stoi(parts[2]);

		int col = pixelX / 100;
		int row = pixelY / 100;
		Position pos(row, col);

		gameState.handleClick(pos);
	}
	else if (command == "wait" && parts.size() == 2) {
		// Wait command - currently just ignore it
		// int ms = std::stoi(parts[1]);
	}
	else if (command == "print" && parts.size() == 2 && parts[1] == "board") {
		gameState.getBoard().print();
	}
}
