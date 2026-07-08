#pragma once

#include <vector>
#include <string>
#include "Board.h"
#include "Types.h"
#include "StringUtils.h"

class BoardParser {
public:
	Board parseBoard();

private:
	void processBoardLine(const std::string& line, Board& board);
	Token parseToken(const std::string& str);
	bool isValidToken(const std::string& token) const;
};
