#include "RealTimeArbiter.h"
#include "../model/Board.h"
#include "../model/Piece.h"
#include <algorithm>

RealTimeArbiter::RealTimeArbiter()
	: gameClock(0), isMoveActive(false), isJumpActive(false), kingWasCaptured(false),
	capturedKingColor(Color::NONE), lastMoveDestination(-1, -1) {}

bool RealTimeArbiter::hasActiveMotion() const {
	return isMoveActive;
}

bool RealTimeArbiter::hasActiveJump() const {
	return isJumpActive;
}

void RealTimeArbiter::startMotion(Piece* piece, Position from, Position to, int duration) {
	currentMove.movingPiece = piece;
	currentMove.sourcePos = from;
	currentMove.destinationPos = to;
	currentMove.startTime = gameClock;
	currentMove.arrivalTime = gameClock + duration;
	isMoveActive = true;
}

void RealTimeArbiter::startJump(Piece* piece, Position cell, int duration) {
	currentJump.jumpingPiece = piece;
	currentJump.cell = cell;
	currentJump.landTime = gameClock + duration;
	isJumpActive = true;
}

void RealTimeArbiter::advanceTime(int ms, Board& board) {
	gameClock += ms;
	processMoveArrivals(board);
	processJumpLandings(board);
}

void RealTimeArbiter::processMoveArrivals(Board& board) {
	if (!isMoveActive || gameClock < currentMove.arrivalTime) {
		return;
	}

	// Airborne-defense dodge: if the defender is jumping and hasn't landed
	// yet, the moving piece gets captured and removed from the source
	// instead of completing its move.
	Piece* defenderAtDestination = board.getPieceAt(currentMove.destinationPos);
	bool airborneDefense = isJumpActive
		&& currentJump.jumpingPiece == defenderAtDestination
		&& defenderAtDestination != nullptr
		&& defenderAtDestination->getColor() != currentMove.movingPiece->getColor()
		&& gameClock <= currentJump.landTime;

	if (airborneDefense) {
		resolveAirborneDefenseCapture(board);
	}
	else {
		resolveNormalArrival(board, defenderAtDestination);
	}

	isMoveActive = false;
}

void RealTimeArbiter::resolveAirborneDefenseCapture(Board& board) {
	if (currentMove.movingPiece->getType() == PieceType::KING) {
		kingWasCaptured = true;
		capturedKingColor = currentMove.movingPiece->getColor();
	}
	board.removePieceAt(currentMove.sourcePos);
	isJumpActive = false;
	resetLastMoveDestination();
}

void RealTimeArbiter::resolveNormalArrival(Board& board, Piece* defenderAtDestination) {
	if (defenderAtDestination != nullptr && defenderAtDestination->getType() == PieceType::KING) {
		kingWasCaptured = true;
		capturedKingColor = defenderAtDestination->getColor();
	}

	completedMove.has = true;
	completedMove.color = currentMove.movingPiece->getColor();
	completedMove.type = currentMove.movingPiece->getType();
	completedMove.source = currentMove.sourcePos;
	completedMove.destination = currentMove.destinationPos;
	completedMove.wasCapture = defenderAtDestination != nullptr;
	completedMove.capturedType = completedMove.wasCapture ? defenderAtDestination->getType() : PieceType::NONE;
	completedMove.gameClockMs = gameClock;

	board.movePieceOnBoard(currentMove.sourcePos, currentMove.destinationPos);
	lastMoveDestination = currentMove.destinationPos;
}

void RealTimeArbiter::processJumpLandings(Board& board) {
	if (isJumpActive && gameClock >= currentJump.landTime) {
		isJumpActive = false;
	}
}

bool RealTimeArbiter::consumeKingWasCaptured(Color& outCapturedColor) {
	if (kingWasCaptured) {
		kingWasCaptured = false;
		outCapturedColor = capturedKingColor;
		return true;
	}
	return false;
}

Position RealTimeArbiter::getLastMoveDestination() const {
	return lastMoveDestination;
}

void RealTimeArbiter::resetLastMoveDestination() {
	lastMoveDestination = Position(-1, -1);
}

bool RealTimeArbiter::consumeCompletedMove(CompletedMoveInfo& outInfo) {
	if (completedMove.has) {
		outInfo = completedMove;
		completedMove.has = false;
		return true;
	}
	return false;
}

ActiveMoveInfo RealTimeArbiter::getActiveMoveInfo() const {
	ActiveMoveInfo info;
	info.has = isMoveActive;
	if (!info.has) {
		return info;
	}

	info.piece = currentMove.movingPiece;
	info.source = currentMove.sourcePos;
	info.destination = currentMove.destinationPos;

	int totalDuration = currentMove.arrivalTime - currentMove.startTime;
	if (totalDuration <= 0) {
		info.progress = 1.0;
	}
	else {
		double progress = static_cast<double>(gameClock - currentMove.startTime) / totalDuration;
		info.progress = std::min(1.0, std::max(0.0, progress));
	}

	return info;
}
