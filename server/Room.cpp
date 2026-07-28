#include "Room.h"
#include "../core/Constants.h"
#include "../core/model/Piece.h"
#include "../core/protocol/Protocol.h"
#include <iostream>

namespace {
	std::string seatName(Color seat) {
		if (seat == Color::White) return "White";
		if (seat == Color::Black) return "Black";
		return "viewer";
	}
}

Room::Room(std::string roomId, Board board, NetworkServer& networkServer)
	: roomId(std::move(roomId)), board(std::move(board)), engine(this->board), networkServer(networkServer) {
}

Color Room::join(const std::string& connectionId, ix::WebSocket& webSocket, const std::string& username) {
	{
		std::lock_guard<std::mutex> lock(connectionsMutex);
		connectionIds.insert(connectionId);
	}

	Color seat = reclaimDisconnectedSeat(connectionId, username);
	if (seat != Color::NONE) {
		std::cout << username << " reconnected to room " << roomId << " as " << seatName(seat) << "\n";
		broadcastDisconnectCleared();
	}
	else {
		seat = assignSeat(connectionId);
		std::cout << "Assigned " << seatName(seat) << " to " << username << " in room " << roomId << "\n";

		if (seat != Color::NONE) {
			std::lock_guard<std::mutex> lock(namesMutex);
			if (seat == Color::White) {
				whiteName = username;
			}
			else {
				blackName = username;
			}
		}
	}

	webSocket.send(Protocol::encodeRoomJoined(roomId, seat));
	broadcastPlayers();
	return seat;
}

Color Room::assignSeat(const std::string& connectionId) {
	std::lock_guard<std::mutex> lock(seatsMutex);

	Color seat = Color::NONE;
	if (!whiteTaken) {
		seat = Color::White;
		whiteTaken = true;
	}
	else if (!blackTaken) {
		seat = Color::Black;
		blackTaken = true;
	}

	seatsByConnectionId[connectionId] = seat;
	return seat;
}

Color Room::seatFor(const std::string& connectionId) const {
	std::lock_guard<std::mutex> lock(seatsMutex);
	auto it = seatsByConnectionId.find(connectionId);
	return it != seatsByConnectionId.end() ? it->second : Color::NONE;
}

Color Room::reclaimDisconnectedSeat(const std::string& connectionId, const std::string& username) {
	std::lock_guard<std::mutex> lock(disconnectMutex);

	std::string whiteNameCopy, blackNameCopy;
	{
		std::lock_guard<std::mutex> namesLock(namesMutex);
		whiteNameCopy = whiteName;
		blackNameCopy = blackName;
	}

	Color reclaimed = Color::NONE;
	if (whiteDisconnect.active && whiteNameCopy == username) {
		reclaimed = Color::White;
		whiteDisconnect.active = false;
	}
	else if (blackDisconnect.active && blackNameCopy == username) {
		reclaimed = Color::Black;
		blackDisconnect.active = false;
	}

	if (reclaimed == Color::NONE) {
		return Color::NONE;
	}

	std::lock_guard<std::mutex> seatsLock(seatsMutex);
	seatsByConnectionId[connectionId] = reclaimed;
	return reclaimed;
}

void Room::handleMove(const std::string& connectionId, ix::WebSocket& webSocket, const std::string& text) {
	Protocol::MoveCommand command = Protocol::decodeMoveCommand(text);
	if (!command.isValid) {
		return;
	}

	Color seat = seatFor(connectionId);
	if (seat == Color::NONE) {
		// Viewers can't move anything - still tell them so the client can
		// flash the same "rejected" feedback a player would get.
		webSocket.send(Protocol::encodeRejection(command.destination));
		return;
	}

	bool accepted;
	{
		std::lock_guard<std::mutex> lock(engineMutex);
		Piece* movingPiece = engine.getBoard().getPieceAt(command.source);
		if (movingPiece == nullptr || movingPiece->getColor() != seat) {
			accepted = false; // can only move your own pieces
		}
		else {
			accepted = engine.requestMove(command.source, command.destination).isAccepted;
		}
	}

	if (!accepted) {
		webSocket.send(Protocol::encodeRejection(command.destination));
	}
}

void Room::onClose(const std::string& connectionId) {
	Color seat;
	{
		std::lock_guard<std::mutex> lock(seatsMutex);
		auto it = seatsByConnectionId.find(connectionId);
		seat = it != seatsByConnectionId.end() ? it->second : Color::NONE;
		seatsByConnectionId.erase(connectionId);
	}

	{
		std::lock_guard<std::mutex> lock(connectionsMutex);
		connectionIds.erase(connectionId);
	}

	if (seat != Color::NONE) {
		startResignCountdown(seat);
	}
}

void Room::startResignCountdown(Color seat) {
	std::lock_guard<std::mutex> lock(disconnectMutex);

	std::lock_guard<std::mutex> engineLock(engineMutex);
	if (engine.isGameOver()) {
		return; // nothing to resign into
	}

	DisconnectState& state = seat == Color::White ? whiteDisconnect : blackDisconnect;
	if (state.active) {
		return; // this seat is already counting down
	}

	state.active = true;
	state.deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(AUTO_RESIGN_MS);
}

void Room::checkAutoResign() {
	// White and Black are independent - both may be mid-disconnect at once.
	checkSeatResign(Color::White);
	checkSeatResign(Color::Black);
}

void Room::checkSeatResign(Color seat) {
	bool shouldResign = false;
	int remainingMs = 0;

	{
		std::lock_guard<std::mutex> lock(disconnectMutex);
		DisconnectState& state = seat == Color::White ? whiteDisconnect : blackDisconnect;
		if (!state.active) {
			return;
		}

		auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
			state.deadline - std::chrono::steady_clock::now()).count();

		if (remaining <= 0) {
			shouldResign = true;
			state.active = false;
		}
		else {
			remainingMs = static_cast<int>(remaining);
		}
	}

	if (shouldResign) {
		std::lock_guard<std::mutex> lock(engineMutex);
		// If both seats' grace periods happened to expire the same tick,
		// the first call already ended the game - skip resigning again.
		if (!engine.isGameOver()) {
			engine.resign(seat);
			std::cout << seatName(seat) << " didn't reconnect in time in room " << roomId << " - auto-resigned\n";
		}
	}
	else {
		broadcastDisconnectCountdown(seat, remainingMs);
	}
}

void Room::broadcastToRoom(const std::string& message) {
	std::set<std::string> ids;
	{
		std::lock_guard<std::mutex> lock(connectionsMutex);
		ids = connectionIds;
	}
	networkServer.broadcastTo(ids, message);
}

void Room::broadcastPlayers() {
	std::string message;
	{
		std::lock_guard<std::mutex> lock(namesMutex);
		message = Protocol::encodePlayers(whiteName, blackName);
	}
	broadcastToRoom(message);
}

void Room::broadcastDisconnectCountdown(Color color, int remainingMs) {
	broadcastToRoom(Protocol::encodeDisconnectCountdown(color, remainingMs));
}

void Room::broadcastDisconnectCleared() {
	broadcastToRoom(Protocol::encodeDisconnectCleared());
}

void Room::broadcastSnapshot() {
	std::string message;
	{
		std::lock_guard<std::mutex> lock(engineMutex);
		message = Protocol::encodeSnapshot(engine.snapshot());
	}
	broadcastToRoom(message);
}

void Room::tick(int elapsedMs) {
	{
		std::lock_guard<std::mutex> lock(engineMutex);
		engine.advanceTime(elapsedMs);
	}

	checkAutoResign();
	broadcastSnapshot();
}

bool Room::isEmpty() const {
	std::lock_guard<std::mutex> lock(connectionsMutex);
	if (!connectionIds.empty()) {
		return false;
	}

	std::lock_guard<std::mutex> disconnectLock(disconnectMutex);
	return !whiteDisconnect.active && !blackDisconnect.active;
}
