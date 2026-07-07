#include "BoardParser.h"
#include <iostream>
#include <sstream>
#include <algorithm>

Board BoardParser::parseInput() {
	Board board;
	std::string line;
	bool reading_board = false;
	bool reading_commands = false;

	while (std::getline(std::cin, line)) {
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}

		if (line == "Board:") {
			reading_board = true;
			reading_commands = false;
			continue;
		}

		if (line == "Commands:") {
			reading_board = false;
			reading_commands = true;
			continue;
		}

		if (reading_board) {
			std::vector<std::string> tokens = splitLine(line);
			if (tokens.empty()) {
				continue;
			}

			for (const std::string& token : tokens) {
				if (!isValidToken(token)) {
					std::cout << "ERROR UNKNOWN_TOKEN\n";
					return board;
				}
			}

			std::vector<Token> row;
			for (const std::string& token : tokens) {
				row.push_back(parseToken(token));
			}

			if (!board.addRow(row)) {
				std::cout << "ERROR ROW_WIDTH_MISMATCH\n";
				return board;
			}
		}

		if (reading_commands) {
			if (line == "print board") {
				board.print();
			}
		}
	}

	return board;
}

Token BoardParser::parseToken(const std::string& str) {
	Token token;
	token.color = Color::NONE;
	token.piece = Piece::NONE;

	if (str == ".") {
		return token;
	}

	token.color = (str[0] == 'w') ? Color::White : Color::Black;

	switch (str[1]) {
	case 'K':
		token.piece = Piece::King;
		break;
	case 'Q':
		token.piece = Piece::Queen;
		break;
	case 'R':
		token.piece = Piece::Rook;
		break;
	case 'N':
		token.piece = Piece::Knight;
		break;
	case 'P':
		token.piece = Piece::Pawn;
		break;
	default:
		token.piece = Piece::NONE;
		break;
	}

	return token;
}

std::vector<std::string> BoardParser::splitLine(const std::string& line) {
	std::vector<std::string> tokens;
	std::stringstream ss(line);
	std::string token;
	while (ss >> token) {
		tokens.push_back(token);
	}
	return tokens;
}

bool BoardParser::isValidToken(const std::string& token) const {
	if (token == ".") {
		return true;
	}
	if (token.length() != 2) {
		return false;
	}

	char color = token[0];
	char piece = token[1];

	bool is_valid_color = (std::find(COLOR_SYMBOLS + 1, COLOR_SYMBOLS + 3, color) != COLOR_SYMBOLS + 3);
	bool is_valid_piece = (std::find(PIECE_SYMBOLS + 1, PIECE_SYMBOLS + 6, piece) != PIECE_SYMBOLS + 6);

	return is_valid_color && is_valid_piece;
}
