#pragma once

#include <string>

// All the fixed rendering numbers and asset paths in one place, so nothing
// has to be re-derived or copy-pasted between Renderer.cpp and CTD.cpp (the
// composition root).

// Fixed rendering constant from the course spec: every board cell is
// CELL_SIZE x CELL_SIZE pixels. Shared by PieceGraphicsLibrary (resizes
// loaded sprites to this) and Renderer (positions them on the canvas).
inline constexpr int CELL_SIZE = 100;

// Blank space reserved on each side of the board for future UI (score
// tables, move log, player names). Sized so the total canvas (~1920px)
// already matches a common screen width - the window then opens "full"
// natively, with no runtime stretching (and thus no click scale-factor to
// get wrong).
inline constexpr int SIDE_PANEL_WIDTH = 455;

// Breathing room between the board grid and the canvas edge/side panels.
inline constexpr int BOARD_MARGIN = 40;

// Path (relative to PROJECT_ROOT_DIR) to the checkerboard image Renderer
// draws as the gameplay grid - already baked to an exact 800x800 (8x8 at
// CELL_SIZE each), so it's loaded and drawn directly, with no cropping or
// per-cell resizing needed at runtime.
inline const std::string BOARD_IMAGE_PATH = "assets/board.png";
