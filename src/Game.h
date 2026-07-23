#pragma once

#include "client/NetworkClient.h"
#include "input/Controller.h"
#include "input/GameWindow.h"
#include "view/animation/PieceGraphicsLibrary.h"
#include "view/Renderer.h"
#include <chrono>
#include <string>

// Owns every long-lived piece of the live graphical client (network
// connection, input, rendering) and runs its main loop. main() (in CTD.cpp)
// only passes the server's address and hands off here - everything else
// (fullscreen setup, per-frame input/render, exit) lives in this class.
//
// The client never owns a GameEngine/Board: it only ever sees the latest
// GameSnapshot the server sent, and only ever sends move requests - the
// server is the single source of truth for game state and legality.
class Game {
public:
	Game(const std::string& serverUrl, const std::string& username);

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
	// timing concern tracked here in the composition root, not inside
	// NetworkClient/Controller. trigger() starts (or restarts) the
	// countdown; tick() counts it down every frame; dismiss() hides it
	// immediately (any new click ends a still-showing marker early,
	// instead of leaving it up to look stale next to a new selection).
	struct RejectionMarker {
		bool showing = false;
		Position position;
		int remainingMs = 0;
		void trigger(const Position& pos);
		void tick(int elapsedMs);
		void dismiss() { showing = false; }
	};

	// Undoes the fullscreen letterbox scale/offset on a raw click, then
	// subtracts the board's own margin - so what Controller/BoardMapper see
	// is a plain board-local pixel, exactly as if the frame were still
	// shown at its native (unscaled) size. Neither of them ever sees a
	// screen pixel.
	void dispatchClick(int clickX, int clickY, const GameSnapshot& snapshot);

	NetworkClient networkClient;
	Controller controller;
	GameWindow window;
	PieceGraphicsLibrary library;
	Renderer renderer;

	std::string boardImagePath;
	std::string gameOverImagePath;
	int boardWidth;
	FullscreenLayout fsLayout;
	RejectionMarker rejectionMarker;
	std::chrono::steady_clock::time_point lastTick;
};
