#include "BoardParser.h"
#include "../model/Piece.h"
#include <iostream>
#include <sstream>
#include <algorithm>

Board BoardParser::parseBoard() {
	Board board;
	std::string line;

	while (std::getline(std::cin, line)) {
		trimCarriageReturn(line);

		if (line.find("Board:") != std::string::npos) {
			break;
		}
	}

	while (std::getline(std::cin, line)) {
		trimCarriageReturn(line);

		if (line.find("Commands:") != std::string::npos) {
			return board;
		}

		if (!line.empty()) {
			processBoardLine(line, board);
		}
	}

	return board;
}

void BoardParser::processBoardLine(const std::string& line, Board& board) {
	std::vector<std::string> tokens = splitLine(line);

	if (tokens.empty()) {
		return;
	}

	for (const std::string& token : tokens) {
		if (!isValidToken(token)) {
			std::cout << "ERROR UNKNOWN_TOKEN\n";
			exit(0);
		}
	}

	std::vector<std::unique_ptr<Piece>> row;
	for (const std::string& token : tokens) {
		row.push_back(parseToken(token));
	}

	if (!board.addRow(row)) {
		std::cout << "ERROR ROW_WIDTH_MISMATCH\n";
		exit(0);
	}
}

std::unique_ptr<Piece> BoardParser::parseToken(const std::string& str) {
	if (str == ".") {
		return nullptr;
	}

	Color color = PieceNotation::parseColor(str[0]);
	PieceType type = PieceNotation::parsePieceType(str[1]);

	if (type == PieceType::NONE) {
		return nullptr;
	}

	return std::make_unique<Piece>(color, type);
}

bool BoardParser::isValidToken(const std::string& token) const {
	if (token == ".") {
		return true;
	}
	if (token.length() != 2) {
		return false;
	}

	bool is_valid_color = PieceNotation::parseColor(token[0]) != Color::NONE;
	bool is_valid_piece = PieceNotation::parsePieceType(token[1]) != PieceType::NONE;

	return is_valid_color && is_valid_piece;
}
