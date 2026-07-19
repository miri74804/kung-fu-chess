#include "Renderer.h"
#include "RenderConstants.h"

namespace {
	// A translucent solid-color square the size of one cell - built via
	// Img::blank (no rectangle primitive needed) and alpha-blended on with
	// the usual draw_on, so it reads as a soft highlight over the board/
	// piece beneath it instead of a flat opaque block.
	Img highlightTile(const cv::Scalar& colorWithAlpha) {
		return Img::blank(CELL_SIZE, CELL_SIZE, colorWithAlpha);
	}

	// Every pixel measurement needed to place the gameplay grid on the
	// canvas, derived purely from boardWidth - so render() and
	// marginXPx()/marginYPx() (called independently, from CTD.cpp, before/
	// without a render() having run yet) always agree.
	struct BoardLayout {
		int gridWidthPx;
		int gridHeightPx;
		int boardOffsetX; // canvas pixel where gameplay grid col 0 starts
		int boardOffsetY;
		int canvasWidth;
		int canvasHeight;
	};

	BoardLayout computeLayout(int boardWidth, int boardHeight) {
		BoardLayout layout;
		layout.gridWidthPx = boardWidth * CELL_SIZE;
		layout.gridHeightPx = boardHeight * CELL_SIZE;

		layout.boardOffsetX = SIDE_PANEL_WIDTH + BOARD_MARGIN;
		layout.boardOffsetY = BOARD_MARGIN;

		layout.canvasWidth = layout.gridWidthPx + 2 * SIDE_PANEL_WIDTH + 2 * BOARD_MARGIN;
		layout.canvasHeight = layout.gridHeightPx + 2 * BOARD_MARGIN;
		return layout;
	}
}

Renderer::Renderer(const PieceGraphicsLibrary& library) : director(library) {}

Img Renderer::render(const std::string& boardImagePath, const GameSnapshot& snapshot, int elapsedMs,
	bool hasSelection, const Position& selectedPosition,
	bool hasRejection, const Position& rejectedPosition) {
	director.advance(elapsedMs, snapshot);

	BoardLayout layout = computeLayout(snapshot.boardWidth, snapshot.boardHeight);
	int boardOffsetX = layout.boardOffsetX;
	int boardOffsetY = layout.boardOffsetY;

	if (baseCanvasCacheSize != layout.gridWidthPx) {
		// Already baked to an exact 800x800 (8x8 cells at CELL_SIZE each) -
		// loaded and drawn as-is, no cropping or resizing needed.
		Img grid;
		grid.read(boardImagePath);

		baseCanvasCache = Img::blank(layout.canvasWidth, layout.canvasHeight);
		grid.draw_on(baseCanvasCache, boardOffsetX, boardOffsetY);
		baseCanvasCacheSize = layout.gridWidthPx;
	}

	// A cheap pixel copy of the pre-composited board, instead of redoing
	// the (expensive - measured ~230ms) alpha-blend of board.png every frame.
	Img canvas = baseCanvasCache.clone();

	// The path the active move travels through, drawn under the pieces so
	// a piece flying over a path cell still reads clearly. Soft green,
	// like a "legal move" highlight.
	Img pathTile = highlightTile(cv::Scalar(0, 200, 0, 110));
	for (const Position& cell : snapshot.activeMovePath) {
		int cellX = boardOffsetX + cell.col * CELL_SIZE;
		int cellY = boardOffsetY + cell.row * CELL_SIZE;
		pathTile.draw_on(canvas, cellX, cellY);
	}

	for (const PieceSnapshot& piece : snapshot.pieces) {
		Img frame = director.frameFor(piece);

		int cellX = boardOffsetX + static_cast<int>(piece.col * CELL_SIZE + 0.5);
		int cellY = boardOffsetY + static_cast<int>(piece.row * CELL_SIZE + 0.5);

		// Sprites are loaded with keep_aspect=true, so a piece narrower or
		// shorter than the cell would otherwise sit pinned to its
		// top-left corner instead of looking centered in it.
		int offsetX = (CELL_SIZE - frame.get_mat().cols) / 2;
		int offsetY = (CELL_SIZE - frame.get_mat().rows) / 2;
		frame.draw_on(canvas, cellX + offsetX, cellY + offsetY);
	}

	// Selection: soft yellow highlight over the whole cell.
	if (hasSelection) {
		int cellX = boardOffsetX + selectedPosition.col * CELL_SIZE;
		int cellY = boardOffsetY + selectedPosition.row * CELL_SIZE;
		Img tile = highlightTile(cv::Scalar(0, 220, 255, 130));
		tile.draw_on(canvas, cellX, cellY);
	}

	// Rejected move attempt: soft red highlight, same treatment.
	if (hasRejection) {
		int cellX = boardOffsetX + rejectedPosition.col * CELL_SIZE;
		int cellY = boardOffsetY + rejectedPosition.row * CELL_SIZE;
		Img tile = highlightTile(cv::Scalar(0, 0, 220, 140));
		tile.draw_on(canvas, cellX, cellY);
	}

	return canvas;
}

int Renderer::marginXPx(int boardWidth) const {
	// Assumes a square board (boardWidth == boardHeight) - true for chess,
	// and computeLayout only uses boardWidth for its scale anyway.
	return computeLayout(boardWidth, boardWidth).boardOffsetX;
}

int Renderer::marginYPx(int boardWidth) const {
	return computeLayout(boardWidth, boardWidth).boardOffsetY;
}

int Renderer::canvasWidthPx(int boardWidth) const {
	return computeLayout(boardWidth, boardWidth).canvasWidth;
}

int Renderer::canvasHeightPx(int boardWidth) const {
	return computeLayout(boardWidth, boardWidth).canvasHeight;
}
