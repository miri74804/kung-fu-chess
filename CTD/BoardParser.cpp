#include "BoardParser.h"
#include "model/Piece.h"
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

	Color color = (str[0] == 'w') ? Color::White : Color::Black;

	switch (str[1]) {
	case 'K':
		return std::make_unique<Piece>(color, PieceType::KING);
	case 'Q':
		return std::make_unique<Piece>(color, PieceType::QUEEN);
	case 'R':
		return std::make_unique<Piece>(color, PieceType::ROOK);
	case 'B':
		return std::make_unique<Piece>(color, PieceType::BISHOP);
	case 'N':
		return std::make_unique<Piece>(color, PieceType::KNIGHT);
	case 'P':
		return std::make_unique<Piece>(color, PieceType::PAWN);
	default:
		return nullptr;
	}
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

	const std::string VALID_PIECES = "KQRBNP";

	bool is_valid_color = (color == 'w' || color == 'b');
	bool is_valid_piece = (VALID_PIECES.find(piece) != std::string::npos);

	return is_valid_color && is_valid_piece;
}
