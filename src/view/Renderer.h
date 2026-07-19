#pragma once

#include "animation/PieceGraphicsLibrary.h"
#include "animation/AnimationDirector.h"
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
	// animations progress frame by frame between calls. hasRejection/
	// rejectedPosition flash a marker over the last illegal move attempt.
	Img render(const std::string& boardImagePath, const GameSnapshot& snapshot, int elapsedMs,
		bool hasSelection, const Position& selectedPosition,
		bool hasRejection, const Position& rejectedPosition);

	// Pixel offsets from the canvas's top-left corner to cell (0,0), for
	// translating raw window clicks into board-grid pixels before handing
	// them to Controller/BoardMapper (which assume the grid starts at 0,0).
	// Separate on purpose: the side panels only push the board right, not
	// down, so X and Y need different offsets.
	int marginXPx(int boardWidth) const;
	int marginYPx(int boardWidth) const;

	// The full rendered frame's own pixel size (before any fullscreen
	// scaling CTD.cpp may apply on top) - needed there to compute how much
	// to scale the frame to fit the real screen, and to invert that scale
	// back off of a raw click before it reaches marginXPx/marginYPx above.
	int canvasWidthPx(int boardWidth) const;
	int canvasHeightPx(int boardWidth) const;

private:
	AnimationDirector director;

	// board.png is expensive to both decode from disk AND alpha-blend onto
	// a canvas (~230ms alone, measured - it never actually changes between
	// frames). baseCanvasCache holds the already-composited blank+board
	// image, built once; every render() just clone()s it (a cheap pixel
	// copy) and draws only the pieces on top of that fresh copy.
	Img baseCanvasCache;
	int baseCanvasCacheSize = -1;
};
