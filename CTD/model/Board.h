#pragma once

#include <vector>
#include <memory>
#include "../Types.h"
#include "Piece.h"

class Board {
private:
	std::vector<std::vector<std::unique_ptr<Piece>>> board;
	int width;
	int height;

public:
	Board();

	bool addRow(std::vector<std::unique_ptr<Piece>>& row);

	Piece* getPieceAt(const Position& pos) const;
	bool isValidPosition(const Position& pos) const;	
	void movePieceOnBoard(const Position& from, const Position& to);
	void removePieceAt(const Position& pos);
	void promoteToQueen(const Position& pos);
	int getWidth() const;
	int getHeight() const;
};
