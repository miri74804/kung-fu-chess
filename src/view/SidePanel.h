#pragma once

#include "Img.h"
#include "../model/Color.h"
#include "../model/MoveLogEntry.h"
#include <string>
#include <vector>

// Draws one player's panel: name, running score, and a Time/Move table of
// their own moves (newest at the bottom; oldest ones scrolled off the top
// once there are more than fit in panelHeightPx - there's no scrolling UI,
// so only the most recent moves are ever shown). Every line is centered
// within the panel's own width (not pinned to its left edge), so it sits
// with even breathing room on both sides instead of hugging the screen
// edge or the board.
class SidePanel {
public:
	static void draw(Img& target, int panelX, int panelWidthPx, int panelHeightPx, Color color,
		const std::string& playerName, int score, const std::vector<MoveLogEntry>& moveLog);
};
