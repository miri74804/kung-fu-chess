#pragma once

#include <vector>
#include <string>
#include "Board.h"
#include "Types.h"

class BoardParser {
public:
	Board parseInput();

private:
	Token parseToken(const std::string& str);
	std::vector<std::string> splitLine(const std::string& line);
	bool isValidToken(const std::string& token) const;
};
