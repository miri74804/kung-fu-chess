#pragma once

// Fixed rendering constant from the course spec: every board cell is
// CELL_SIZE x CELL_SIZE pixels. Shared by PieceGraphicsLibrary (resizes
// loaded sprites to this) and Renderer (positions them on the canvas).
inline constexpr int CELL_SIZE = 100;
