#include "RealTimeArbiter.h"
#include "../model/Board.h"
#include "../model/Piece.h"

RealTimeArbiter::RealTimeArbiter() 
	: gameClock(0), isMoveActive(false), isJumpActive(false), kingWasCaptured(false), lastMoveDestination(-1, -1) {}

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
	if (isMoveActive && gameClock >= currentMove.arrivalTime) {
		Piece* defenderAtDestination = board.getPieceAt(currentMove.destinationPos);

		// Airborne-defense dodge: if the defender is jumping and hasn't landed yet,
		// the moving piece gets captured and removed from the source.
		bool airborneDefense = isJumpActive
			&& currentJump.jumpingPiece == defenderAtDestination
			&& defenderAtDestination != nullptr
			&& defenderAtDestination->getColor() != currentMove.movingPiece->getColor()
			&& gameClock <= currentJump.landTime;

		if (airborneDefense) {
			// Moving piece was captured by the airborne defender.
			// Check if the moving piece is a king.
			if (currentMove.movingPiece->getSymbol() == 'K') {
				kingWasCaptured = true;
			}
			// Remove the moving piece from the source.
			board.removePieceAt(currentMove.sourcePos);
			isJumpActive = false;
			lastMoveDestination = Position(-1, -1);
		}
		else {
			// Normal move completion: move the piece and check for capture.
			if (defenderAtDestination != nullptr && defenderAtDestination->getSymbol() == 'K') {
				kingWasCaptured = true;
			}
			board.movePieceOnBoard(currentMove.sourcePos, currentMove.destinationPos);
			lastMoveDestination = currentMove.destinationPos;

			// Promotion is checked by the caller (GameEngine) after advanceTime,
			// via lastMoveDestination and GameEngine::checkPawnPromotion().
		}

		isMoveActive = false;
	}
}

void RealTimeArbiter::processJumpLandings(Board& board) {
	if (isJumpActive && gameClock >= currentJump.landTime) {
		isJumpActive = false;
	}
}

bool RealTimeArbiter::consumeKingWasCaptured() {
	if (kingWasCaptured) {
		kingWasCaptured = false;
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

int RealTimeArbiter::getGameClock() const {
	return gameClock;
}
