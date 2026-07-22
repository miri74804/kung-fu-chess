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
inline const std::string GAME_OVER_IMAGE_PATH = "assets/game_over.png"; // banner shown centered when the game ends

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

// --- Piece values (for the captured-pieces score) ---
inline constexpr int PAWN_VALUE = 1;
inline constexpr int KNIGHT_VALUE = 3;
inline constexpr int BISHOP_VALUE = 3;
inline constexpr int ROOK_VALUE = 5;
inline constexpr int QUEEN_VALUE = 9;

// --- Piece assets ---
inline constexpr int SPRITES_PER_STATE = 5; // every animation state's sprites/ folder holds exactly this many numbered frames

// --- Animation ---
inline constexpr double FRACTIONAL_EPSILON = 0.01; // precision tolerance for detecting a piece mid-flight (fractional row/col) vs settled on a whole cell

// --- View layout (plain numbers only - cv::Scalar colors stay local to
// the .cpp file that draws with them) ---
inline constexpr double COORD_FONT_SIZE = 0.6;
inline constexpr int COORD_THICKNESS = 1;
inline constexpr double PANEL_HEADER_FONT_SIZE = 0.9;
inline constexpr double PANEL_SCORE_FONT_SIZE = 0.7;
inline constexpr double PANEL_ROW_FONT_SIZE = 0.55;
inline constexpr int PANEL_HEADER_THICKNESS = 2;
inline constexpr int PANEL_ROW_THICKNESS = 1;
inline constexpr int PANEL_ROW_HEIGHT = 28;
inline constexpr int PANEL_TOP_MARGIN = 30;
inline constexpr int PANEL_COLUMN_SPACING = 24; // breathing room between the Time and Move columns
