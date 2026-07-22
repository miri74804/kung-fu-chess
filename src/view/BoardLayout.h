#pragma once

// Every pixel measurement needed to place the gameplay grid on the
// canvas, derived purely from boardWidth/boardHeight - so Renderer and
// marginXPx()/marginYPx() (called independently, from Game.cpp, before/
// without a render() having run yet) always agree.
struct BoardLayout {
	int gridWidthPx;
	int gridHeightPx;
	int boardOffsetX; 
	int boardOffsetY;
	int canvasWidth;
	int canvasHeight;

	static BoardLayout forBoardSize(int boardWidth, int boardHeight);
};
