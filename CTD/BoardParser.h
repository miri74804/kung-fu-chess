#pragma once

#include <vector>
#include <string>
#include <memory>
#include "Board.h"
#include "Types.h"
#include "StringUtils.h"
#include "Piece.h"

class BoardParser {
public:
	Board parseBoard();

private:
	void processBoardLine(const std::string& line, Board& board);
	std::unique_ptr<Piece> parseToken(const std::string& str);
	bool isValidToken(const std::string& token) const;
};
