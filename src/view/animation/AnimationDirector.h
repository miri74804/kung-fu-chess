#pragma once

#include "AnimationState.h"
#include "PieceGraphicsLibrary.h"
#include "../../engine/GameSnapshot.h"
#include <map>
#include <optional>
#include <utility>

// Decides which frame to draw for every piece in a snapshot.
//
// Idle pieces of the same (color, type) look identical, so they share one
// idle clock - no need to track each piece's identity. The common route
// only ever allows one active motion at a time, so there's at most one
// piece "in flight"; that one gets its own clock, created when it starts
// moving and dropped once it arrives.
class AnimationDirector {
public:
	explicit AnimationDirector(const PieceGraphicsLibrary& library);

	// Advances every clock by ms; uses the snapshot to notice when the one
	// active move starts or ends.
	void advance(int ms, const GameSnapshot& snapshot);

	const Img& frameFor(const PieceSnapshot& piece) const;

private:
	const PieceGraphicsLibrary& library;
	std::map<std::pair<Color, PieceType>, AnimationState> idleClocks;
	std::optional<AnimationState> moverClock;

	// A piece mid-flight has a fractional row/col (e.g. row=3.5); an idle
	// or arrived piece sits exactly on a cell (row=3.0).
	static bool isFractional(const PieceSnapshot& piece);
};
