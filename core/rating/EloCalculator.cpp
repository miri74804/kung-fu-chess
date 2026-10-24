#include "EloCalculator.h"
#include "../Constants.h"
#include <cmath>

namespace {
	double actualScore(Color side, Color winner) {
		if (winner == Color::NONE) {
			return 0.5;
		}
		return winner == side ? 1.0 : 0.0;
	}
}

EloResult EloCalculator::computeNewRatings(int whiteRating, int blackRating, Color winner) {
	double expectedWhite = 1.0 / (1.0 + std::pow(10.0, (blackRating - whiteRating) / 400.0));
	double expectedBlack = 1.0 - expectedWhite;

	double actualWhite = actualScore(Color::White, winner);
	double actualBlack = actualScore(Color::Black, winner);

	int newWhiteRating = whiteRating + static_cast<int>(std::lround(ELO_K_FACTOR * (actualWhite - expectedWhite)));
	int newBlackRating = blackRating + static_cast<int>(std::lround(ELO_K_FACTOR * (actualBlack - expectedBlack)));

	return { newWhiteRating, newBlackRating };
}
