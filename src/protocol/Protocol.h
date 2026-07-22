#pragma once

#include "../engine/GameSnapshot.h"
#include "../model/Position.h"
#include <string>

// The only place that knows what a network message looks like: converts
// between the game's internal types (GameSnapshot, a move) and JSON text.
// Sits above notation/engine (like notation/ sits above model/) - neither
// the client nor the server hand-rolls text parsing over its socket.
class Protocol {
public:
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
};
