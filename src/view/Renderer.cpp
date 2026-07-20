#include "Renderer.h"
#include "../Constants.h"

namespace {
	// A translucent solid-color square the size of one cell - built via
	// Img::blank (no rectangle primitive needed) and alpha-blended on with
	// the usual draw_on, so it reads as a soft highlight over the board/
	// piece beneath it instead of a flat opaque block.
	Img highlightTile(const cv::Scalar& colorWithAlpha) {
		return Img::blank(CELL_SIZE, CELL_SIZE, colorWithAlpha);
	}

	// All colors below are BGR(+alpha), matching cv::Scalar's channel order.
	const cv::Scalar PATH_HIGHLIGHT_COLOR(0, 200, 0, 110);       // soft green: cells the active move travels through
	const cv::Scalar SELECTION_HIGHLIGHT_COLOR(0, 220, 255, 130); // soft yellow: the currently selected piece's cell
	const cv::Scalar REJECTION_HIGHLIGHT_COLOR(0, 0, 220, 140);   // soft red: the last rejected move attempt

	// Warm cream/gold - matches the board's own light squares, so the
	// coordinate labels read as part of the same palette instead of a
	// random accent color.
	const cv::Scalar COORD_LABEL_COLOR(170, 205, 227, 255);
	constexpr double COORD_FONT_SIZE = 0.6;
	constexpr int COORD_THICKNESS = 1;

	const cv::Scalar GAME_OVER_OVERLAY_COLOR(0, 0, 0, 160);

	// Draws file letters (a, b, c, ...) above and below the grid, and rank
	// numbers (boardHeight down to 1) to its left and right, each centered
	// in the BOARD_MARGIN-wide strip already reserved around the grid - no
	// extra space needed. Drawn once into the cached base canvas (not per
	// frame), since the board's size never changes mid-game.
	void drawCoordinateLabels(Img& target, int boardOffsetX, int boardOffsetY,
		int gridWidthPx, int gridHeightPx, int boardWidth, int boardHeight) {
		for (int col = 0; col < boardWidth; ++col) {
			std::string label(1, static_cast<char>('a' + col));
			cv::Size textSize = Img::measureText(label, COORD_FONT_SIZE, COORD_THICKNESS);
			int labelX = boardOffsetX + col * CELL_SIZE + (CELL_SIZE - textSize.width) / 2;

			int topY = (BOARD_MARGIN + textSize.height) / 2;
			target.put_text(label, labelX, topY, COORD_FONT_SIZE, COORD_LABEL_COLOR, COORD_THICKNESS);

			int bottomY = boardOffsetY + gridHeightPx + (BOARD_MARGIN + textSize.height) / 2;
			target.put_text(label, labelX, bottomY, COORD_FONT_SIZE, COORD_LABEL_COLOR, COORD_THICKNESS);
		}

		for (int row = 0; row < boardHeight; ++row) {
			std::string label = std::to_string(boardHeight - row);
			cv::Size textSize = Img::measureText(label, COORD_FONT_SIZE, COORD_THICKNESS);
			int labelY = boardOffsetY + row * CELL_SIZE + (CELL_SIZE + textSize.height) / 2;

			int leftX = boardOffsetX - BOARD_MARGIN + (BOARD_MARGIN - textSize.width) / 2;
			target.put_text(label, leftX, labelY, COORD_FONT_SIZE, COORD_LABEL_COLOR, COORD_THICKNESS);

			int rightX = boardOffsetX + gridWidthPx + (BOARD_MARGIN - textSize.width) / 2;
			target.put_text(label, rightX, labelY, COORD_FONT_SIZE, COORD_LABEL_COLOR, COORD_THICKNESS);
		}
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

Img Renderer::render(const std::string& boardImagePath, const std::string& gameOverImagePath,
	const GameSnapshot& snapshot, int elapsedMs,
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
		drawCoordinateLabels(baseCanvasCache, boardOffsetX, boardOffsetY,
			layout.gridWidthPx, layout.gridHeightPx, snapshot.boardWidth, snapshot.boardHeight);
		baseCanvasCacheSize = layout.gridWidthPx;
	}

	// A cheap pixel copy of the pre-composited board, instead of redoing
	// the (expensive - measured ~230ms) alpha-blend of board.png every frame.
	Img canvas = baseCanvasCache.clone();

	// The path the active move travels through, drawn under the pieces so
	// a piece flying over a path cell still reads clearly.
	Img pathTile = highlightTile(PATH_HIGHLIGHT_COLOR);
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

	if (hasSelection) {
		int cellX = boardOffsetX + selectedPosition.col * CELL_SIZE;
		int cellY = boardOffsetY + selectedPosition.row * CELL_SIZE;
		Img tile = highlightTile(SELECTION_HIGHLIGHT_COLOR);
		tile.draw_on(canvas, cellX, cellY);
	}

	if (hasRejection) {
		int cellX = boardOffsetX + rejectedPosition.col * CELL_SIZE;
		int cellY = boardOffsetY + rejectedPosition.row * CELL_SIZE;
		Img tile = highlightTile(REJECTION_HIGHLIGHT_COLOR);
		tile.draw_on(canvas, cellX, cellY);
	}

	if (snapshot.gameOver) {
		if (!gameOverBannerLoaded) {
			gameOverBanner.read(gameOverImagePath);
			gameOverBannerLoaded = true;
		}

		Img overlay = Img::blank(layout.canvasWidth, layout.canvasHeight, GAME_OVER_OVERLAY_COLOR);
		overlay.draw_on(canvas, 0, 0);

		int bannerX = (layout.canvasWidth - gameOverBanner.get_mat().cols) / 2;
		int bannerY = (layout.canvasHeight - gameOverBanner.get_mat().rows) / 2;
		gameOverBanner.draw_on(canvas, bannerX, bannerY);
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
