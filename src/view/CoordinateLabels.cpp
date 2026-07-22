#include "CoordinateLabels.h"
#include "../Constants.h"

namespace {
	const cv::Scalar COORD_LABEL_COLOR(170, 205, 227, 255);
}

void CoordinateLabels::draw(Img& target, int boardOffsetX, int boardOffsetY,
	int gridWidthPx, int gridHeightPx, int boardWidth, int boardHeight) {
	for (int col = 0; col < boardWidth; ++col) {
		std::string label(1, static_cast<char>('a' + col));
		cv::Size textSize = Img::measureText(label, COORD_FONT_SIZE, COORD_THICKNESS);
		int labelX = boardOffsetX + col * CELL_SIZE + (CELL_SIZE - textSize.width) / 2;

		int topY = (BOARD_MARGIN + textSize.height) / 2;
		target.put_text(label, labelX, topY, COORD_FONT_SIZE, COORD_LABEL_COLOR, COORD_THICKNESS);

		int bottomY = boardOffsetY + gridHeightPx + (BOARD_MARGIN + textSize.height) / 2;
		target.put_text(label, labelX, bottomY, COORD_FONT_SIZE, COORD_LABEL_COLOR, COORD_THICKNESS);
	}

	for (int row = 0; row < boardHeight; ++row) {
		std::string label = std::to_string(boardHeight - row);
		cv::Size textSize = Img::measureText(label, COORD_FONT_SIZE, COORD_THICKNESS);
		int labelY = boardOffsetY + row * CELL_SIZE + (CELL_SIZE + textSize.height) / 2;

		int leftX = boardOffsetX - BOARD_MARGIN + (BOARD_MARGIN - textSize.width) / 2;
		target.put_text(label, leftX, labelY, COORD_FONT_SIZE, COORD_LABEL_COLOR, COORD_THICKNESS);

		int rightX = boardOffsetX + gridWidthPx + (BOARD_MARGIN - textSize.width) / 2;
		target.put_text(label, rightX, labelY, COORD_FONT_SIZE, COORD_LABEL_COLOR, COORD_THICKNESS);
	}
}
