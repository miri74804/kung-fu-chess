#include "SidePanel.h"
#include "../Constants.h"
#include <cstdio>

namespace {
	// Same warm cream/gold as the coordinate labels, so the side panels read
	// as part of the same palette.
	const cv::Scalar PANEL_TEXT_COLOR(170, 205, 227, 255);

	std::string formatElapsedTime(int elapsedMs) {
		int totalSeconds = elapsedMs / 1000;
		int minutes = totalSeconds / 60;
		int seconds = totalSeconds % 60;
		int millis = elapsedMs % 1000;

		char buffer[16];
		std::snprintf(buffer, sizeof(buffer), "%02d:%02d.%03d", minutes, seconds, millis);
		return std::string(buffer);
	}
}

void SidePanel::draw(Img& target, int panelX, int panelWidthPx, int panelHeightPx, Color color,
	const std::string& playerName, int score, const std::vector<MoveLogEntry>& moveLog) {
	int panelCenterX = panelX + panelWidthPx / 2;
	int y = PANEL_TOP_MARGIN;

	cv::Size headerSize = Img::measureText(playerName, PANEL_HEADER_FONT_SIZE, PANEL_HEADER_THICKNESS);
	target.put_text(playerName, panelCenterX - headerSize.width / 2, y, PANEL_HEADER_FONT_SIZE, PANEL_TEXT_COLOR, PANEL_HEADER_THICKNESS);
	y += PANEL_ROW_HEIGHT;

	std::string scoreText = "Score: " + std::to_string(score);
	cv::Size scoreSize = Img::measureText(scoreText, PANEL_SCORE_FONT_SIZE, PANEL_ROW_THICKNESS);
	target.put_text(scoreText, panelCenterX - scoreSize.width / 2, y, PANEL_SCORE_FONT_SIZE, PANEL_TEXT_COLOR, PANEL_ROW_THICKNESS);
	y += static_cast<int>(PANEL_ROW_HEIGHT * 1.5);

	// Fixed-width representative strings (the actual format never produces
	// anything wider), so every row's columns land at the exact same x as
	// the header - measuring each row's own text instead would make the
	// columns drift left/right per row.
	cv::Size timeColSize = Img::measureText("00:00.000", PANEL_ROW_FONT_SIZE, PANEL_ROW_THICKNESS);
	cv::Size moveColSize = Img::measureText("Qxh8", PANEL_ROW_FONT_SIZE, PANEL_ROW_THICKNESS);
	int tableWidth = timeColSize.width + PANEL_COLUMN_SPACING + moveColSize.width;
	int timeColX = panelCenterX - tableWidth / 2;
	int moveColX = timeColX + timeColSize.width + PANEL_COLUMN_SPACING;

	target.put_text("Time", timeColX, y, PANEL_ROW_FONT_SIZE, PANEL_TEXT_COLOR, PANEL_ROW_THICKNESS);
	target.put_text("Move", moveColX, y, PANEL_ROW_FONT_SIZE, PANEL_TEXT_COLOR, PANEL_ROW_THICKNESS);
	y += PANEL_ROW_HEIGHT;

	std::vector<const MoveLogEntry*> ownMoves;
	for (const MoveLogEntry& entry : moveLog) {
		if (entry.color == color) {
			ownMoves.push_back(&entry);
		}
	}

	int maxRows = (panelHeightPx - y) / PANEL_ROW_HEIGHT;
	int startIndex = static_cast<int>(ownMoves.size()) > maxRows
		? static_cast<int>(ownMoves.size()) - maxRows
		: 0;

	for (int i = startIndex; i < static_cast<int>(ownMoves.size()); ++i) {
		const MoveLogEntry& entry = *ownMoves[i];
		target.put_text(formatElapsedTime(entry.elapsedMs), timeColX, y, PANEL_ROW_FONT_SIZE, PANEL_TEXT_COLOR, PANEL_ROW_THICKNESS);
		target.put_text(entry.notation, moveColX, y, PANEL_ROW_FONT_SIZE, PANEL_TEXT_COLOR, PANEL_ROW_THICKNESS);
		y += PANEL_ROW_HEIGHT;
	}
}
