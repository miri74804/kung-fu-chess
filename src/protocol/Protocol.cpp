#include "Protocol.h"
#include "../notation/PieceNotation.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {
	json positionToJson(const Position& pos) {
		return json{ {"row", pos.row}, {"col", pos.col} };
	}

	Position positionFromJson(const json& j) {
		return Position(j.at("row").get<int>(), j.at("col").get<int>());
	}

	// Colors/piece types travel as the same single-char symbols the notation
	// layer already defines (w/b, K/Q/R/B/N/P) - one enum<->char mapping,
	// reused instead of inventing a second one just for JSON.
	std::string colorToJson(Color color) { return std::string(1, PieceNotation::toSymbol(color)); }
	std::string pieceTypeToJson(PieceType type) { return std::string(1, PieceNotation::toSymbol(type)); }
}

std::string Protocol::encodeSnapshot(const GameSnapshot& snapshot) {
	json j;
	j["type"] = "snapshot";
	j["boardWidth"] = snapshot.boardWidth;
	j["boardHeight"] = snapshot.boardHeight;
	j["gameOver"] = snapshot.gameOver;
	j["winner"] = colorToJson(snapshot.winner);
	j["whiteScore"] = snapshot.whiteScore;
	j["blackScore"] = snapshot.blackScore;

	json pieces = json::array();
	for (const PieceSnapshot& piece : snapshot.pieces) {
		pieces.push_back({
			{"color", colorToJson(piece.color)},
			{"type", pieceTypeToJson(piece.type)},
			{"row", piece.row},
			{"col", piece.col}
		});
	}
	j["pieces"] = pieces;

	json path = json::array();
	for (const Position& cell : snapshot.activeMovePath) {
		path.push_back(positionToJson(cell));
	}
	j["activeMovePath"] = path;

	json moveLog = json::array();
	for (const MoveLogEntry& entry : snapshot.moveLog) {
		moveLog.push_back({
			{"color", colorToJson(entry.color)},
			{"notation", entry.notation},
			{"elapsedMs", entry.elapsedMs}
		});
	}
	j["moveLog"] = moveLog;

	return j.dump();
}

GameSnapshot Protocol::decodeSnapshot(const std::string& message) {
	json j = json::parse(message);

	GameSnapshot snapshot;
	snapshot.boardWidth = j.at("boardWidth").get<int>();
	snapshot.boardHeight = j.at("boardHeight").get<int>();
	snapshot.gameOver = j.at("gameOver").get<bool>();
	snapshot.winner = PieceNotation::parseColor(j.at("winner").get<std::string>()[0]);
	snapshot.whiteScore = j.at("whiteScore").get<int>();
	snapshot.blackScore = j.at("blackScore").get<int>();

	for (const json& p : j.at("pieces")) {
		PieceSnapshot piece;
		piece.color = PieceNotation::parseColor(p.at("color").get<std::string>()[0]);
		piece.type = PieceNotation::parsePieceType(p.at("type").get<std::string>()[0]);
		piece.row = p.at("row").get<double>();
		piece.col = p.at("col").get<double>();
		snapshot.pieces.push_back(piece);
	}

	for (const json& cell : j.at("activeMovePath")) {
		snapshot.activeMovePath.push_back(positionFromJson(cell));
	}

	for (const json& entry : j.at("moveLog")) {
		MoveLogEntry logEntry;
		logEntry.color = PieceNotation::parseColor(entry.at("color").get<std::string>()[0]);
		logEntry.notation = entry.at("notation").get<std::string>();
		logEntry.elapsedMs = entry.at("elapsedMs").get<int>();
		snapshot.moveLog.push_back(logEntry);
	}

	return snapshot;
}

std::string Protocol::encodeMoveCommand(const Position& source, const Position& destination) {
	json j;
	j["type"] = "move";
	j["from"] = positionToJson(source);
	j["to"] = positionToJson(destination);
	return j.dump();
}

Protocol::MoveCommand Protocol::decodeMoveCommand(const std::string& message) {
	try {
		json j = json::parse(message);
		if (j.at("type").get<std::string>() != "move") {
			return { false, Position(), Position() };
		}
		return { true, positionFromJson(j.at("from")), positionFromJson(j.at("to")) };
	}
	catch (const json::exception&) {
		return { false, Position(), Position() };
	}
}
