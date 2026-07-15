#pragma once

#include <string>
#include "ScriptCommand.h"

class ScriptParser {
public:
	ScriptCommand parse(const std::string& line) const;
};
