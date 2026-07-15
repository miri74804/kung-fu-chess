#pragma once

#include <vector>
#include <memory>
#include "Position.h"
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
	void setPieceAt(const Position& pos, std::unique_ptr<Piece> newPiece);
	int getWidth() const;
	int getHeight() const;
};
