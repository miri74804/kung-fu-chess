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

	// Sent once by a client right after connecting: username + password.
	// The server creates the account (rating = STARTING_ELO) the first
	// time a username is seen, or verifies the password against the
	// existing account otherwise - either way, the reply is login_ok/
	// login_error, never a silent failure.
	static std::string encodeLogin(const std::string& username, const std::string& password);
	struct Login {
		bool isValid;
		std::string username;
		std::string password;
	};
	static Login decodeLogin(const std::string& message);

	// Sent once by the server in reply to a successful login: the
	// (possibly just-created) account's current rating.
	static std::string encodeLoginOk(const std::string& username, int rating);
	struct LoginOk {
		bool isValid;
		std::string username;
		int rating;
	};
	static LoginOk decodeLoginOk(const std::string& message);

	// Sent once by the server instead of login_ok when the password didn't
	// match an existing account.
	static std::string encodeLoginError(const std::string& reason);
	struct LoginError {
		bool isValid;
		std::string reason;
	};
	static LoginError decodeLoginError(const std::string& message);

	// Sent once by an already-logged-in client: create a brand-new room
	// (server generates the id) and join it as the first seat. No username
	// travels here - the server already knows who this connection is from
	// its earlier login.
	static std::string encodeCreateRoom();
	struct CreateRoom {
		bool isValid;
	};
	static CreateRoom decodeCreateRoom(const std::string& message);

	// Sent once by an already-logged-in client: join an existing room by id
	// (second joiner becomes Black, everyone after that is a viewer -
	// decided server-side, not carried here).
	static std::string encodeJoinRoom(const std::string& roomId);
	struct JoinRoom {
		bool isValid;
		std::string roomId;
	};
	static JoinRoom decodeJoinRoom(const std::string& message);

	// Sent once by the server in reply to a successful create_room/join_room:
	// which room the client is now in, and which side it controls.
	// Color::NONE means "viewer" - reuses PieceNotation's existing '.'/'w'/'b'
	// symbols rather than inventing a separate viewer flag.
	static std::string encodeRoomJoined(const std::string& roomId, Color color);
	struct RoomJoined {
		bool isValid;
		std::string roomId;
		Color color;
	};
	static RoomJoined decodeRoomJoined(const std::string& message);

	// Sent once by the server in reply to a join_room that named an unknown
	// room id, instead of room_joined.
	static std::string encodeRoomError(const std::string& reason);
	struct RoomError {
		bool isValid;
		std::string reason;
	};
	static RoomError decodeRoomError(const std::string& message);

	// Broadcast by the server (within one room) whenever either seat's
	// display name changes (i.e. right after a create_room/join_room is
	// processed), so every connected client - including viewers - can show
	// real names/ratings instead of the generic "White"/"Black" fallback.
	static std::string encodePlayers(const std::string& whiteName, int whiteRating, const std::string& blackName, int blackRating);
	struct Players {
		bool isValid;
		std::string whiteName;
		int whiteRating;
		std::string blackName;
		int blackRating;
	};
	static Players decodePlayers(const std::string& message);
};
