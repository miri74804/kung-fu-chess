#include "Renderer.h"
#include "rendering/CoordinateLabels.h"
#include "rendering/SidePanel.h"
#include "../../core/Constants.h"

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
	const cv::Scalar GAME_OVER_OVERLAY_COLOR(0, 0, 0, 160);
	const cv::Scalar DISCONNECT_WARNING_BG_COLOR(0, 0, 0, 190);
	const cv::Scalar DISCONNECT_WARNING_TEXT_COLOR(0, 220, 255, 255); // amber - reads as a warning, distinct from the panels' cream text
	const cv::Scalar ROOM_BANNER_BG_COLOR(0, 0, 0, 190);
	const cv::Scalar ROOM_BANNER_TEXT_COLOR(255, 255, 255, 255);

	// None of the functions below touch a Renderer's own member state
	// (director, the cached banner) - they only read their arguments and
	// draw onto canvas, so they're free functions instead of private
	// methods (see Renderer.h for the two steps that DO need member access).

	// Drawn under the pieces so a piece flying over a path cell still
	// reads clearly.
	void drawActiveMovePath(Img& canvas, const BoardLayout& layout, const GameSnapshot& snapshot) {
		Img pathTile = highlightTile(PATH_HIGHLIGHT_COLOR);
		for (const Position& cell : snapshot.activeMovePath) {
			int cellX = layout.boardOffsetX + cell.col * CELL_SIZE;
			int cellY = layout.boardOffsetY + cell.row * CELL_SIZE;
			pathTile.draw_on(canvas, cellX, cellY);
		}
	}

	// Shared by both the selection marker and the rejection flash - same
	// "one highlighted cell" shape, just a different color/position/condition.
	void drawTileHighlight(Img& canvas, const BoardLayout& layout, const Position& pos, const cv::Scalar& color) {
		int cellX = layout.boardOffsetX + pos.col * CELL_SIZE;
		int cellY = layout.boardOffsetY + pos.row * CELL_SIZE;
		Img tile = highlightTile(color);
		tile.draw_on(canvas, cellX, cellY);
	}

	void drawDisconnectWarning(Img& canvas, const BoardLayout& layout, Color disconnectedColor, int remainingMs,
		const std::string& whiteName, const std::string& blackName) {
		const std::string& disconnectedName = disconnectedColor == Color::White ? whiteName : blackName;
		std::string text = disconnectedName + " disconnected - auto-resign in "
			+ std::to_string((remainingMs + 999) / 1000) + "s";
		cv::Size textSize = Img::measureText(text, DISCONNECT_WARNING_FONT_SIZE, DISCONNECT_WARNING_THICKNESS);
		int textX = (layout.canvasWidth - textSize.width) / 2;
		int textY = layout.boardOffsetY + textSize.height + 10;

		int padding = 12;
		Img background = Img::blank(textSize.width + padding * 2, textSize.height + padding * 2, DISCONNECT_WARNING_BG_COLOR);
		background.draw_on(canvas, textX - padding, textY - textSize.height - padding);
		canvas.put_text(text, textX, textY, DISCONNECT_WARNING_FONT_SIZE, DISCONNECT_WARNING_TEXT_COLOR, DISCONNECT_WARNING_THICKNESS);
	}

	// Bottom of the Black side panel's own column (x in [0, SIDE_PANEL_WIDTH])
	// - below where its name/score/move-table content ever reaches, and
	// nowhere near the board itself (unlike the board's own top-left corner,
	// which sits right under the a8/h8 pieces).
	void drawRoomBanner(Img& canvas, const BoardLayout& layout, const std::string& roomId) {
		std::string text = "Room: " + roomId;
		cv::Size textSize = Img::measureText(text, PANEL_ROW_FONT_SIZE, PANEL_ROW_THICKNESS);

		int padding = 6;
		int badgeWidth = textSize.width + padding * 2;
		int badgeHeight = textSize.height + padding * 2;
		int badgeX = (SIDE_PANEL_WIDTH - badgeWidth) / 2;
		int badgeY = layout.canvasHeight - badgeHeight - 10;
		int textX = badgeX + padding;
		int textY = badgeY + padding + textSize.height;

		Img background = Img::blank(badgeWidth, badgeHeight, ROOM_BANNER_BG_COLOR);
		background.draw_on(canvas, badgeX, badgeY);
		canvas.put_text(text, textX, textY, PANEL_ROW_FONT_SIZE, ROOM_BANNER_TEXT_COLOR, PANEL_ROW_THICKNESS);
	}
}

Renderer::Renderer(const PieceGraphicsLibrary& library) : director(library) {}

void Renderer::drawPieces(Img& canvas, const BoardLayout& layout, const GameSnapshot& snapshot) {
	for (const PieceSnapshot& piece : snapshot.pieces) {
		Img frame = director.frameFor(piece);

		int cellX = layout.boardOffsetX + static_cast<int>(piece.col * CELL_SIZE + 0.5);
		int cellY = layout.boardOffsetY + static_cast<int>(piece.row * CELL_SIZE + 0.5);

		// Sprites are loaded with keep_aspect=true, so a piece narrower or
		// shorter than the cell would otherwise sit pinned to its
		// top-left corner instead of looking centered in it.
		int offsetX = (CELL_SIZE - frame.get_mat().cols) / 2;
		int offsetY = (CELL_SIZE - frame.get_mat().rows) / 2;
		frame.draw_on(canvas, cellX + offsetX, cellY + offsetY);
	}
}

void Renderer::drawGameOverBanner(Img& canvas, const BoardLayout& layout, const std::string& gameOverImagePath) {
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

Img Renderer::render(const std::string& boardImagePath, const std::string& gameOverImagePath,
	const GameSnapshot& snapshot, int elapsedMs,
	bool hasSelection, const Position& selectedPosition,
	bool hasRejection, const Position& rejectedPosition,
	bool hasDisconnectWarning, Color disconnectedColor, int disconnectRemainingMs,
	const std::string& whiteName, const std::string& blackName,
	const std::string& roomId) {
	director.advance(elapsedMs, snapshot);

	BoardLayout layout = BoardLayout::forBoardSize(snapshot.boardWidth, snapshot.boardHeight);

	if (baseCanvasCacheSize != layout.gridWidthPx) {
		// Already baked to an exact 800x800 (8x8 cells at CELL_SIZE each) -
		// loaded and drawn as-is, no cropping or resizing needed.
		Img grid;
		grid.read(boardImagePath);

		baseCanvasCache = Img::blank(layout.canvasWidth, layout.canvasHeight);
		grid.draw_on(baseCanvasCache, layout.boardOffsetX, layout.boardOffsetY);
		CoordinateLabels::draw(baseCanvasCache, layout.boardOffsetX, layout.boardOffsetY,
			layout.gridWidthPx, layout.gridHeightPx, snapshot.boardWidth, snapshot.boardHeight);
		baseCanvasCacheSize = layout.gridWidthPx;
	}

	// A cheap pixel copy of the pre-composited board, instead of redoing
	// the (expensive - measured ~230ms) alpha-blend of board.png every frame.
	Img canvas = baseCanvasCache.clone();

	drawActiveMovePath(canvas, layout, snapshot);
	drawPieces(canvas, layout, snapshot);

	if (hasSelection) {
		drawTileHighlight(canvas, layout, selectedPosition, SELECTION_HIGHLIGHT_COLOR);
	}

	if (hasRejection) {
		drawTileHighlight(canvas, layout, rejectedPosition, REJECTION_HIGHLIGHT_COLOR);
	}

	SidePanel::draw(canvas, 0, SIDE_PANEL_WIDTH, layout.canvasHeight, Color::Black, blackName, snapshot.blackScore, snapshot.moveLog);
	int rightPanelX = layout.boardOffsetX + layout.gridWidthPx + BOARD_MARGIN;
	SidePanel::draw(canvas, rightPanelX, SIDE_PANEL_WIDTH, layout.canvasHeight, Color::White, whiteName, snapshot.whiteScore, snapshot.moveLog);

	if (hasDisconnectWarning && !snapshot.gameOver) {
		drawDisconnectWarning(canvas, layout, disconnectedColor, disconnectRemainingMs, whiteName, blackName);
	}

	if (snapshot.gameOver) {
		drawGameOverBanner(canvas, layout, gameOverImagePath);
	}

	if (!roomId.empty()) {
		drawRoomBanner(canvas, layout, roomId);
	}

	return canvas;
}

int Renderer::marginXPx(int boardWidth) const {
	// Assumes a square board (boardWidth == boardHeight) - true for chess,
	// and BoardLayout::forBoardSize only uses boardWidth for its scale anyway.
	return BoardLayout::forBoardSize(boardWidth, boardWidth).boardOffsetX;
}

int Renderer::marginYPx(int boardWidth) const {
	return BoardLayout::forBoardSize(boardWidth, boardWidth).boardOffsetY;
}

int Renderer::canvasWidthPx(int boardWidth) const {
	return BoardLayout::forBoardSize(boardWidth, boardWidth).canvasWidth;
}

int Renderer::canvasHeightPx(int boardWidth) const {
	return BoardLayout::forBoardSize(boardWidth, boardWidth).canvasHeight;
}
