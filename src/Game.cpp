// Must come before any header that might pull in <windows.h> (Game.h's own
// chain, via OpenCV, does): without it, windows.h #defines min/max as
// macros, which silently corrupts every std::min/std::max call below into
// a syntax error (the preprocessor rewrites "std::min" textually, with no
// idea it's a namespaced function).
#define NOMINMAX
#include "Game.h"
#include "Constants.h"
#include <windows.h>
#include <algorithm>
#include <iostream>

namespace {
	PieceGraphicsLibrary loadPieceLibrary() {
		PieceGraphicsLibrary library;
		library.loadAll(std::string(PROJECT_ROOT_DIR) + "/assets/pieces");
		return library;
	}
}

Game::Game(Board initialBoard)
	: board(std::move(initialBoard)),
	engine(board),
	library(loadPieceLibrary()),
	renderer(library),
	boardImagePath(std::string(PROJECT_ROOT_DIR) + "/" + BOARD_IMAGE_PATH),
	gameOverImagePath(std::string(PROJECT_ROOT_DIR) + "/" + GAME_OVER_IMAGE_PATH),
	boardWidth(engine.getBoard().getWidth()),
	fsLayout(computeFullscreenLayout(renderer.canvasWidthPx(boardWidth), renderer.canvasHeightPx(boardWidth))),
	lastTick(std::chrono::steady_clock::now()) {
	window.enableFullscreen();
	std::cout << "Click pieces to move them. Press ESC in the window to quit.\n";
}

Game::FullscreenLayout Game::computeFullscreenLayout(int canvasWidth, int canvasHeight) {
	FullscreenLayout layout;
	layout.screenWidth = GetSystemMetrics(SM_CXSCREEN);
	layout.screenHeight = GetSystemMetrics(SM_CYSCREEN);
	layout.scale = std::min(layout.screenWidth / static_cast<double>(canvasWidth),
		layout.screenHeight / static_cast<double>(canvasHeight));
	layout.scaledWidth = static_cast<int>(canvasWidth * layout.scale);
	layout.scaledHeight = static_cast<int>(canvasHeight * layout.scale);
	layout.offsetX = (layout.screenWidth - layout.scaledWidth) / 2;
	layout.offsetY = (layout.screenHeight - layout.scaledHeight) / 2;
	return layout;
}

void Game::dispatchClick(int clickX, int clickY) {
	int frameX = static_cast<int>((clickX - fsLayout.offsetX) / fsLayout.scale);
	int frameY = static_cast<int>((clickY - fsLayout.offsetY) / fsLayout.scale);

	int localX = frameX - renderer.marginXPx(boardWidth);
	int localY = frameY - renderer.marginYPx(boardWidth);
	controller.click(localX, localY, engine);
}

void Game::RejectionMarker::update(bool clickHappened, Controller& controller, int elapsedMs) {
	if (clickHappened) {
		if (controller.wasLastMoveRejected()) {
			showing = true;
			position = controller.getLastRejectedPosition();
			remainingMs = REJECTION_DISPLAY_MS;
		}
		else {
			// Any other click (a new selection, or an accepted move)
			// dismisses a still-showing marker immediately, instead of
			// leaving it up to look stale next to the new selection.
			showing = false;
		}
	}

	if (showing) {
		remainingMs -= elapsedMs;
		if (remainingMs <= 0) {
			showing = false;
		}
	}
}

int Game::run() {
	// Rendering takes longer than any fixed tick guess (e.g. 30ms), so
	// advancing the game clock by a constant every iteration would make it
	// fall behind real time - a "1 second" move would then take several
	// real seconds. Instead, advance by however long the last iteration
	// actually took (capped by MAX_TICK_MS, so one slow frame - e.g. window
	// drag - can't make a piece suddenly jump far ahead).
	while (true) {
		auto now = std::chrono::steady_clock::now();
		int elapsedMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick).count());
		elapsedMs = std::min(elapsedMs, MAX_TICK_MS);
		lastTick = now;

		int clickX, clickY;
		bool clickHappened = window.pollClick(clickX, clickY) && !engine.isMotionInProgress() && !engine.isGameOver();
		if (clickHappened) {
			dispatchClick(clickX, clickY);
		}
		rejectionMarker.update(clickHappened, controller, elapsedMs);

		engine.advanceTime(elapsedMs);

		Img frame = renderer.render(boardImagePath, gameOverImagePath, engine.snapshot(), elapsedMs,
			controller.hasSelection(), controller.getSelectedPosition(),
			rejectionMarker.showing, rejectionMarker.position);

		// Scale our own frame up to fill the screen ourselves (uniformly,
		// centered - never stretched non-uniformly), instead of letting
		// OpenCV's fullscreen property silently stretch it to whatever the
		// screen's aspect ratio happens to be.
		frame.resize(fsLayout.scaledWidth, fsLayout.scaledHeight);
		Img fullscreenFrame = Img::blank(fsLayout.screenWidth, fsLayout.screenHeight, cv::Scalar(0, 0, 0, 255));
		// draw_on_opaque, not draw_on: frame is always fully opaque here, so
		// draw_on's per-pixel alpha blend is pure overhead - at full-screen
		// sizes, expensive enough every single frame to feel like input lag
		// (a click only registers on the next shown frame).
		frame.draw_on_opaque(fullscreenFrame, fsLayout.offsetX, fsLayout.offsetY);

		int key = fullscreenFrame.show(1);
		if (key == ESC_KEY_CODE) {
			break;
		}
	}

	return 0;
}
