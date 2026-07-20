#pragma once

#include "model/Board.h"
#include "engine/GameEngine.h"
#include "input/Controller.h"
#include "input/GameWindow.h"
#include "view/animation/PieceGraphicsLibrary.h"
#include "view/Renderer.h"
#include <chrono>
#include <string>

// Owns every long-lived piece of the live graphical game (engine, input,
// rendering) and runs its main loop. main() (in CTD.cpp) only parses the
// board and hands it here - everything else (fullscreen setup, per-frame
// input/render, exit) lives in this class instead of one long function.
class Game {
public:
	explicit Game(Board initialBoard);

	// Runs until the user closes the window (ESC or the X button).
	// Returns the process exit code.
	int run();

private:
	// Fits a canvasWidth x canvasHeight frame to the real screen
	// resolution: uniformly scaled (never stretched non-uniformly) and
	// centered, with black letterbox bars if the aspect ratios differ.
	struct FullscreenLayout {
		int screenWidth;
		int screenHeight;
		double scale;
		int scaledWidth;
		int scaledHeight;
		int offsetX;
		int offsetY;
	};
	static FullscreenLayout computeFullscreenLayout(int canvasWidth, int canvasHeight);

	// The rejection marker should flash briefly and fade on its own - a
	// timing concern that (per the architecture) is tracked here in the
	// composition root, not inside Controller. update() edge-detects a
	// *new* rejection (Controller's own flag just stays "true" until the
	// next click, however long that takes) and counts down independently.
	struct RejectionMarker {
		bool showing = false;
		Position position;
		int remainingMs = 0;
		void update(bool clickHappened, Controller& controller, int elapsedMs);
	};

	// Undoes the fullscreen letterbox scale/offset on a raw click, then
	// subtracts the board's own margin - so what Controller/BoardMapper see
	// is a plain board-local pixel, exactly as if the frame were still
	// shown at its native (unscaled) size. Neither of them ever sees a
	// screen pixel.
	void dispatchClick(int clickX, int clickY);

	Board board;
	GameEngine engine;
	Controller controller;
	GameWindow window;
	PieceGraphicsLibrary library;
	Renderer renderer;

	std::string boardImagePath;
	int boardWidth;
	FullscreenLayout fsLayout;
	RejectionMarker rejectionMarker;
	std::chrono::steady_clock::time_point lastTick;
};
