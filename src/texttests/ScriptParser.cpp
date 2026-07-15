#include "ScriptParser.h"
#include "../utils/StringUtils.h"

ScriptCommand ScriptParser::parse(const std::string& line) const {
	ScriptCommand command;
	std::vector<std::string> parts = splitLine(line);

	if (parts.empty()) {
		return command;
	}

	const std::string& name = parts[0];

	if (name == "click" && parts.size() == 3) {
		command.type = CommandType::Click;
		command.x = std::stoi(parts[1]);
		command.y = std::stoi(parts[2]);
	}
	else if (name == "jump" && parts.size() == 3) {
		command.type = CommandType::Jump;
		command.x = std::stoi(parts[1]);
		command.y = std::stoi(parts[2]);
	}
	else if (name == "wait" && parts.size() == 2) {
		command.type = CommandType::Wait;
		command.ms = std::stoi(parts[1]);
	}
	else if (name == "print" && parts.size() == 2 && parts[1] == "board") {
		command.type = CommandType::PrintBoard;
	}

	return command;
}
