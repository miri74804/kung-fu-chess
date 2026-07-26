#pragma once

enum class CommandType {
	Click,
	Jump,
	Wait,
	PrintBoard,
	Unknown
};

struct ScriptCommand {
	CommandType type = CommandType::Unknown;
	int x = 0;
	int y = 0;
	int ms = 0;
};
