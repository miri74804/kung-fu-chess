#pragma once

#include <vector>
#include <string>
#include <memory>
#include "../model/Board.h"
#include "../utils/StringUtils.h"
#include "../model/Piece.h"
#include "PieceNotation.h"

class BoardParser {
public:
	Board parseBoard();

private:
	void processBoardLine(const std::string& line, Board& board);
	std::unique_ptr<Piece> parseToken(const std::string& str);
	bool isValidToken(const std::string& token) const;
};
