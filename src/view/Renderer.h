#pragma once

#include "PieceGraphicsLibrary.h"
#include "../engine/GameSnapshot.h"
#include "Img.h"
#include <string>

// Draws a GameSnapshot onto an image: the board background resized to the
// snapshot's pixel size, then every piece at its (possibly fractional,
// mid-flight) cell using its idle sprite. Never touches Board/Piece
// directly - only reads GameSnapshot - so drawing can never mutate game
// state, per the view layer's rules.
class Renderer {
public:
	explicit Renderer(const PieceGraphicsLibrary& library);

	Img render(const std::string& boardImagePath, const GameSnapshot& snapshot) const;

	// Pixel offset from the canvas's top-left corner to cell (0,0), for
	// translating raw window clicks into board-grid pixels before handing
	// them to Controller/BoardMapper (which assume the grid starts at 0,0).
	int marginPx(int boardWidth) const;

private:
	const PieceGraphicsLibrary& library;
};
