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
#include <chrono>
#include <iostream>
#include <thread>

namespace {
	PieceGraphicsLibrary loadPieceLibrary() {
		PieceGraphicsLibrary library;
		library.loadAll(std::string(PROJECT_ROOT_DIR) + "/assets/pieces");
		return library;
	}

	// The board's dimensions aren't known until the server sends its first
	// snapshot - blocks (briefly) rather than starting the window at some
	// guessed size and resizing it once the real board shows up.
	int waitForBoardWidth(NetworkClient& client) {
		std::cout << "Connecting to server..." << std::endl;
		int waitedMs = 0;
		while (!client.hasSnapshot()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			waitedMs += 50;
			if (waitedMs % 2000 == 0) {
				// Still stuck after a couple of seconds - almost always
				// means the server isn't running/reachable at this address.
				std::cout << "Still waiting for the server (" << waitedMs / 1000 << "s)..." << std::endl;
			}
		}
		return client.latestSnapshot().boardWidth;
	}
}

Game::Game(const std::string& serverUrl)
	: networkClient(serverUrl),
	library(loadPieceLibrary()),
	renderer(library),
	boardImagePath(std::string(PROJECT_ROOT_DIR) + "/" + BOARD_IMAGE_PATH),
	gameOverImagePath(std::string(PROJECT_ROOT_DIR) + "/" + GAME_OVER_IMAGE_PATH),
	boardWidth(waitForBoardWidth(networkClient)),
	fsLayout(computeFullscreenLayout(renderer.canvasWidthPx(boardWidth), renderer.canvasHeightPx(boardWidth))),
	lastTick(std::chrono::steady_clock::now()) {
	window.enableFullscreen();
	std::cout << "Connected. Click pieces to move them. Press ESC in the window to quit.\n";
}

void Game::RejectionMarker::trigger(const Position& pos) {
	showing = true;
	position = pos;
	remainingMs = REJECTION_DISPLAY_MS;
}

void Game::RejectionMarker::tick(int elapsedMs) {
	if (showing) {
		remainingMs -= elapsedMs;
		if (remainingMs <= 0) {
			showing = false;
		}
	}
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

void Game::dispatchClick(int clickX, int clickY, const GameSnapshot& snapshot) {
	int frameX = static_cast<int>((clickX - fsLayout.offsetX) / fsLayout.scale);
	int frameY = static_cast<int>((clickY - fsLayout.offsetY) / fsLayout.scale);

	int localX = frameX - renderer.marginXPx(boardWidth);
	int localY = frameY - renderer.marginYPx(boardWidth);

	MoveRequest request = controller.click(localX, localY, snapshot, networkClient.assignedColor());
	if (request.requested) {
		networkClient.sendMove(request.source, request.destination);
	}
}

int Game::run() {
	// Rendering takes longer than any fixed tick guess (e.g. 30ms), so
	// advancing by a constant every iteration would make animation timing
	// fall behind real time. Instead, advance by however long the last
	// iteration actually took (capped by MAX_TICK_MS, so one slow frame -
	// e.g. window drag - can't make a piece suddenly jump far ahead).
	while (true) {
		auto now = std::chrono::steady_clock::now();
		int elapsedMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick).count());
		elapsedMs = std::min(elapsedMs, MAX_TICK_MS);
		lastTick = now;

		GameSnapshot snapshot = networkClient.latestSnapshot();

		Position rejectedPosition;
		if (networkClient.consumeRejection(rejectedPosition)) {
			rejectionMarker.trigger(rejectedPosition);
		}
		rejectionMarker.tick(elapsedMs);

		int clickX, clickY;
		// activeMovePath is only ever non-empty while some piece is mid-flight
		// (MoveGeometry::computePath always lists at least the destination
		// cell for an active move) - reusing it here means a click can't
		// select/attempt a second move while one is still animating, same
		// as the old single-process behavior (engine.isMotionInProgress()).
		bool clickHappened = window.pollClick(clickX, clickY) && !snapshot.gameOver && snapshot.activeMovePath.empty();
		if (clickHappened) {
			// A new click always dismisses a still-showing marker immediately -
			// otherwise it would keep flashing next to whatever the player
			// selects next. If this very click turns into a new rejection,
			// that arrives from the server in a later frame and re-triggers it.
			rejectionMarker.dismiss();
			dispatchClick(clickX, clickY, snapshot);
		}

		Img frame = renderer.render(boardImagePath, gameOverImagePath, snapshot, elapsedMs,
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
