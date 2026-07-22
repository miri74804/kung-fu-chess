#include "BoardLayout.h"
#include "../Constants.h"

BoardLayout BoardLayout::forBoardSize(int boardWidth, int boardHeight) {
	BoardLayout layout;
	layout.gridWidthPx = boardWidth * CELL_SIZE;
	layout.gridHeightPx = boardHeight * CELL_SIZE;

	layout.boardOffsetX = SIDE_PANEL_WIDTH + BOARD_MARGIN;
	layout.boardOffsetY = BOARD_MARGIN;

	layout.canvasWidth = layout.gridWidthPx + 2 * SIDE_PANEL_WIDTH + 2 * BOARD_MARGIN;
	layout.canvasHeight = layout.gridHeightPx + 2 * BOARD_MARGIN;
	return layout;
}
