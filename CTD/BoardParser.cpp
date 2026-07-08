#include "BoardParser.h"
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

	std::vector<Token> row;
	for (const std::string& token : tokens) {
		row.push_back(parseToken(token));
	}

	if (!board.addRow(row)) {
		std::cout << "ERROR ROW_WIDTH_MISMATCH\n";
		exit(0);
	}
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
