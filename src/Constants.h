#pragma once

#include <string>


// View-only constants that need an OpenCV type (e.g. cv::Scalar colors)
// are deliberately NOT here - they stay local to Renderer.cpp, so the
// model/rules/engine layers below never need an OpenCV include just to
// read a plain number. This file has no dependencies of its own, so any
// layer can include it safely.

// --- Board grid ---
inline constexpr int CELL_SIZE = 100;
inline constexpr int SIDE_PANEL_WIDTH = 455; // reserved each side of the board, for future UI (score table, move log)
inline constexpr int BOARD_MARGIN = 40;      // breathing room between the grid and the canvas edge/side panels
inline const std::string BOARD_IMAGE_PATH = "assets/board.png"; // pre-baked, exact CELL_SIZE-per-cell checkerboard

// --- Window ---
// Shared by Img::show()/show(waitMs) (which draw into a window by this
// name) and GameWindow (whose setMouseCallback attaches to a window by
// this same name) - they must match, or click capture silently stops
// working.
inline const std::string GAME_WINDOW_NAME = "Image";
inline constexpr int ESC_KEY_CODE = 27;

// --- Timing (all in milliseconds) ---
inline constexpr int MOVE_DURATION_PER_CELL_MS = 1000; // a sliding move's real-time duration scales by distance
inline constexpr int JUMP_DURATION_MS = 1000;          // how long a piece stays airborne jumping in place - fixed, not distance-based, so kept as its own constant rather than reusing the one above
inline constexpr int MAX_TICK_MS = 100;                // caps one game-loop tick, so a slow frame (e.g. window drag) can't make a piece jump far ahead
inline constexpr int REJECTION_DISPLAY_MS = 2000;       // how long an illegal-move marker stays on screen
