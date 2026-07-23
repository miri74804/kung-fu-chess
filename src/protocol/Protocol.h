#pragma once

#include "../engine/GameSnapshot.h"
#include "../model/Color.h"
#include "../model/Position.h"
#include <string>

// The only place that knows what a network message looks like: converts
// between the game's internal types (GameSnapshot, a move, a color
// assignment) and JSON text. Sits above notation/engine (like notation/
// sits above model/) - neither the client nor the server hand-rolls text
// parsing over its socket.
class Protocol {
public:
	// Reads just the "type" field, so a receiver can decide which decode*
	// to call next - returns "" for anything that isn't a JSON object with
	// a string "type" field.
	static std::string peekType(const std::string& message);

	static std::string encodeSnapshot(const GameSnapshot& snapshot);
	static GameSnapshot decodeSnapshot(const std::string& message);

	static std::string encodeMoveCommand(const Position& source, const Position& destination);

	// isValid is false for anything that isn't a well-formed move command
	// (unknown message type, missing fields, bad JSON) - callers should
	// just ignore the message rather than throw.
	struct MoveCommand {
		bool isValid;
		Position source;
		Position destination;
	};
	static MoveCommand decodeMoveCommand(const std::string& message);

	// Sent once by the server to a newly-connected client: which side it
	// controls. Color::NONE means "viewer" (connected after both White and
	// Black were already taken) - reuses PieceNotation's existing '.'/'w'/'b'
	// symbols rather than inventing a separate viewer flag.
	static std::string encodeAssignment(Color color);
	struct Assignment {
		bool isValid;
		Color color;
	};
	static Assignment decodeAssignment(const std::string& message);

	// Sent by the server to a single client (never broadcast) whenever
	// that client's own move command was rejected - the position clicked
	// as the destination, so the client can flash exactly that cell.
	static std::string encodeRejection(const Position& position);
	struct Rejection {
		bool isValid;
		Position position;
	};
	static Rejection decodeRejection(const std::string& message);

	// Broadcast every tick while a seated player is disconnected and might
	// still auto-resign - which color, and how much of the grace period is
	// left. If the grace period elapses, a resignation shows up as an
	// ordinary GameSnapshot with gameOver=true and this simply stops being
	// sent. If instead the player reconnects in time, the countdown ends
	// WITHOUT a game-over - so the server broadcasts encodeDisconnectCleared()
	// once, or every other client's last-known countdown would stay stuck
	// on screen forever (nothing else would ever tell them it ended).
	static std::string encodeDisconnectCountdown(Color color, int remainingMs);
	struct DisconnectCountdown {
		bool isValid;
		Color color;
		int remainingMs;
	};
	static DisconnectCountdown decodeDisconnectCountdown(const std::string& message);

	static std::string encodeDisconnectCleared();

	// Sent once by a client right after connecting: the username it wants
	// displayed. Just a display label - there's no password/account behind
	// it yet (that's a separate, larger login feature).
	static std::string encodeLogin(const std::string& username);
	struct Login {
		bool isValid;
		std::string username;
	};
	static Login decodeLogin(const std::string& message);

	// Broadcast by the server whenever either seat's display name changes
	// (i.e. right after a login message is processed), so every connected
	// client - including viewers - can show real names instead of the
	// generic "White"/"Black" fallback.
	static std::string encodePlayers(const std::string& whiteName, const std::string& blackName);
	struct Players {
		bool isValid;
		std::string whiteName;
		std::string blackName;
	};
	static Players decodePlayers(const std::string& message);
};
