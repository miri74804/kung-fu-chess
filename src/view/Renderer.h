#pragma once

#include "PieceGraphicsLibrary.h"
#include "AnimationDirector.h"
#include "../engine/GameSnapshot.h"
#include "../model/Position.h"
#include "Img.h"
#include <string>

// Draws a GameSnapshot onto an image: the board background resized to the
// snapshot's pixel size, then every piece at its (possibly fractional,
// mid-flight) cell using the correct animation frame (via AnimationDirector),
// then an optional selection marker. Never touches Board/Piece directly -
// only reads GameSnapshot - so drawing can never mutate game state.
class Renderer {
public:
	explicit Renderer(const PieceGraphicsLibrary& library);

	// elapsedMs is forwarded to the internal AnimationDirector so idle/move
	// animations progress frame by frame between calls.
	Img render(const std::string& boardImagePath, const GameSnapshot& snapshot, int elapsedMs,
		bool hasSelection, const Position& selectedPosition);

	// Pixel offset from the canvas's top-left corner to cell (0,0), for
	// translating raw window clicks into board-grid pixels before handing
	// them to Controller/BoardMapper (which assume the grid starts at 0,0).
	int marginPx(int boardWidth) const;

private:
	AnimationDirector director;
};
