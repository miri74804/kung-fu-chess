#pragma once

#include "../model/Color.h"

// Result of one game, ready to persist - both ratings recomputed together
// since each side's new rating depends on the other's rating too.
struct EloResult {
	int newWhiteRating;
	int newBlackRating;
};

// Pure math, no I/O - the standard chess ELO update formula. Kept separate
// from GameEngine/Room so it's testable in isolation and never needs to
// know about the DB or the network.
class EloCalculator {
public:
	// winner is Color::White/Black for a decisive result, Color::NONE for a
	// draw (each side's actual score is then 0.5 instead of 1/0).
	static EloResult computeNewRatings(int whiteRating, int blackRating, Color winner);
};
