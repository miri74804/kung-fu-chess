// Repo: https://github.com/miri74804/kung-fu-chess
//
// The real, graphical game: parses a board from stdin (same "Board:"
// format as before), then loops - poll for a click, feed it to Controller
// (ignoring clicks while a motion is active - Controller itself stays
// blind to timing, per the architecture), advance game time by however
// long the previous iteration actually took (not a fixed guess - see the
// comment by lastTick below), render, show. ESC closes the window.
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <algorithm>
#include <windows.h>
#include "io/BoardParser.h"
#include "engine/GameEngine.h"
#include "input/Controller.h"
#include "input/GameWindow.h"
#include "view/animation/PieceGraphicsLibrary.h"
#include "view/Renderer.h"
#include "view/RenderConstants.h"

int main() {
	// Without this, Windows silently rescales our window if display scaling
	// (e.g. 125%/150%, common on high-DPI screens) is on - the image then
	// gets stretched on screen and click coordinates stop lining up with
	// its actual pixels (an edge click can land in the next cell over).
	// This just tells Windows "I'll handle my own pixels" - unrelated to
	// Img/drawing rules, it's a one-time OS-level process setting.
	SetProcessDPIAware();

	BoardParser boardParser;
	Board board;

	try {
		board = boardParser.parseBoard();
	}
	catch (const std::runtime_error& e) {
		std::cout << e.what() << "\n";
		return 0;
	}

	GameEngine engine(board);
	Controller controller;
	GameWindow window;

	PieceGraphicsLibrary library;
	library.loadAll(std::string(PROJECT_ROOT_DIR) + "/assets/pieces");
	Renderer renderer(library);

	std::string boardImagePath = std::string(PROJECT_ROOT_DIR) + "/" + BOARD_IMAGE_PATH;

	// Fullscreen, done safely: the window is told to fill the physical
	// screen, but WE control exactly how our own frame gets scaled to fit
	// it (uniformly, letterboxed - never stretched non-uniformly), and we
	// invert that same scale on every click before it reaches marginXPx/
	// marginYPx or BoardMapper. BoardMapper and Controller never see a
	// screen pixel - only a plain board-local pixel, exactly like before.
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	window.enableFullscreen();

	int boardWidth = engine.getBoard().getWidth();
	int canvasWidth = renderer.canvasWidthPx(boardWidth);
	int canvasHeight = renderer.canvasHeightPx(boardWidth);
	double fullscreenScale = std::min(screenWidth / static_cast<double>(canvasWidth),
		screenHeight / static_cast<double>(canvasHeight));
	int scaledCanvasWidth = static_cast<int>(canvasWidth * fullscreenScale);
	int scaledCanvasHeight = static_cast<int>(canvasHeight * fullscreenScale);
	int fullscreenOffsetX = (screenWidth - scaledCanvasWidth) / 2;
	int fullscreenOffsetY = (screenHeight - scaledCanvasHeight) / 2;

	std::cout << "Click pieces to move them. Press ESC in the window to quit.\n";

	// Rendering takes longer than any fixed tick guess (e.g. 30ms), so
	// advancing the game clock by a constant every iteration would make it
	// fall behind real time - a "1 second" move would then take several
	// real seconds. Instead, advance by however long the last iteration
	// actually took (capped, so one slow frame - e.g. window drag - can't
	// make the piece suddenly jump far ahead).
	auto lastTick = std::chrono::steady_clock::now();
	const int MAX_TICK_MS = 100;

	// The rejection marker should flash briefly and fade on its own - that's
	// a timing concern, so (per the architecture) it's tracked here in the
	// composition root, not inside Controller. Edge-detects a *new*
	// rejection (Controller's own flag just stays "true" until the next
	// click, however long that takes) and counts down independently.
	const int REJECTION_DISPLAY_MS = 2000;
	bool showRejectionMarker = false;
	Position rejectionMarkerPos;
	int rejectionMarkerRemainingMs = 0;

	while (true) {
		auto now = std::chrono::steady_clock::now();
		int elapsedMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick).count());
		elapsedMs = std::min(elapsedMs, MAX_TICK_MS);
		lastTick = now;

		int clickX, clickY;
		bool clickHappened = window.pollClick(clickX, clickY) && !engine.isMotionInProgress() && !engine.isGameOver();
		if (clickHappened) {
			// Undo the fullscreen letterbox scale/offset first, so what
			// reaches marginXPx/marginYPx below is a plain pixel in our own
			// frame's coordinate space - exactly as if we were still
			// showing the frame at its native (unscaled) size.
			int frameX = static_cast<int>((clickX - fullscreenOffsetX) / fullscreenScale);
			int frameY = static_cast<int>((clickY - fullscreenOffsetY) / fullscreenScale);

			int mx = renderer.marginXPx(boardWidth);
			int my = renderer.marginYPx(boardWidth);
			int localX = frameX - mx;
			int localY = frameY - my;
			controller.click(localX, localY, engine);
		}

		if (clickHappened) {
			if (controller.wasLastMoveRejected()) {
				showRejectionMarker = true;
				rejectionMarkerPos = controller.getLastRejectedPosition();
				rejectionMarkerRemainingMs = REJECTION_DISPLAY_MS;
			}
			else {
				// Any other click (a new selection, or an accepted move)
				// dismisses a still-showing marker immediately, instead of
				// leaving it up to look stale next to the new selection.
				showRejectionMarker = false;
			}
		}

		if (showRejectionMarker) {
			rejectionMarkerRemainingMs -= elapsedMs;
			if (rejectionMarkerRemainingMs <= 0) {
				showRejectionMarker = false;
			}
		}

		engine.advanceTime(elapsedMs);

		Img frame = renderer.render(boardImagePath, engine.snapshot(), elapsedMs,
			controller.hasSelection(), controller.getSelectedPosition(),
			showRejectionMarker, rejectionMarkerPos);

		// Scale our own frame up to fill the screen ourselves (uniformly,
		// centered - never stretched non-uniformly), instead of letting
		// OpenCV's fullscreen property silently stretch it to whatever the
		// screen's aspect ratio happens to be.
		frame.resize(scaledCanvasWidth, scaledCanvasHeight);
		Img fullscreenFrame = Img::blank(screenWidth, screenHeight, cv::Scalar(0, 0, 0, 255));
		// draw_on_opaque, not draw_on: frame is always fully opaque here, so
		// draw_on's per-pixel alpha blend is pure overhead - at full-screen
		// sizes, expensive enough every single frame to feel like input lag
		// (a click only registers on the next shown frame).
		frame.draw_on_opaque(fullscreenFrame, fullscreenOffsetX, fullscreenOffsetY);

		int key = fullscreenFrame.show(1);
		if (key == 27) { // ESC
			break;
		}
	}

	return 0;
}
