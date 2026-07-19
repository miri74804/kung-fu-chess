#include "Renderer.h"
#include "RenderConstants.h"

namespace {
	// assets/board.png is 1254x1254; the actual 8x8 checkerboard only starts
	// 131px in from every side (transparent PNG padding plus the decorative
	// gold/wood frame - measured via demo/inspect_board_demo.cpp). Everything
	// below scales those two numbers so pieces land on the real grid instead
	// of assuming the image is the grid edge-to-edge.
	constexpr int BOARD_IMAGE_SIZE = 1254;
	constexpr int BOARD_IMAGE_MARGIN = 131;

	struct BoardGeometry {
		int canvasSize;
		int margin;
	};

	BoardGeometry computeGeometry(int boardWidth) {
		int gridPx = BOARD_IMAGE_SIZE - 2 * BOARD_IMAGE_MARGIN;
		double scale = (boardWidth * CELL_SIZE) / static_cast<double>(gridPx);

		BoardGeometry geometry;
		geometry.canvasSize = static_cast<int>(BOARD_IMAGE_SIZE * scale + 0.5);
		geometry.margin = static_cast<int>(BOARD_IMAGE_MARGIN * scale + 0.5);
		return geometry;
	}
}

Renderer::Renderer(const PieceGraphicsLibrary& library) : director(library) {}

Img Renderer::render(const std::string& boardImagePath, const GameSnapshot& snapshot, int elapsedMs,
	bool hasSelection, const Position& selectedPosition) {
	director.advance(elapsedMs, snapshot);

	BoardGeometry geometry = computeGeometry(snapshot.boardWidth);

	Img canvas;
	canvas.read(boardImagePath, { geometry.canvasSize, geometry.canvasSize });

	for (const PieceSnapshot& piece : snapshot.pieces) {
		Img frame = director.frameFor(piece);

		int x = geometry.margin + static_cast<int>(piece.col * CELL_SIZE + 0.5);
		int y = geometry.margin + static_cast<int>(piece.row * CELL_SIZE + 0.5);
		frame.draw_on(canvas, x, y);
	}

	// Selection marker: Img has no rectangle primitive, so this is a crude
	// "+" for now (a real highlight overlay image, made in Paint.NET per
	// the course slides, can replace this later). The +4/+24 pixel offsets
	// just nudge the glyph roughly toward the cell's center.
	if (hasSelection) {
		int cellX = geometry.margin + selectedPosition.col * CELL_SIZE;
		int cellY = geometry.margin + selectedPosition.row * CELL_SIZE;
		canvas.put_text("+", cellX + 4, cellY + 24, 1.0, cv::Scalar(0, 255, 255, 255), 3);
	}

	return canvas;
}

int Renderer::marginPx(int boardWidth) const {
	return computeGeometry(boardWidth).margin;
}
