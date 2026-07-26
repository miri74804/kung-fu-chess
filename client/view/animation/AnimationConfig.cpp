#include "AnimationConfig.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace {
	// The course's config.json always has this fixed, flat shape (two nested
	// objects, four known leaf fields) - a hand-rolled scan is enough and
	// avoids pulling in a JSON library for a format this small and trusted.
	std::string readWholeFile(const std::string& path) {
		std::ifstream file(path);
		if (!file) {
			throw std::runtime_error("Cannot open config file: " + path);
		}
		std::ostringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}

	// Finds "key" in json and returns the raw text of its value (trimmed,
	// quotes stripped if present), or "" if the key isn't found.
	std::string extractRawValue(const std::string& json, const std::string& key) {
		std::string needle = "\"" + key + "\"";
		size_t keyPos = json.find(needle);
		if (keyPos == std::string::npos) {
			return "";
		}
		size_t colonPos = json.find(':', keyPos + needle.size());
		if (colonPos == std::string::npos) {
			return "";
		}
		size_t valueStart = colonPos + 1;
		size_t valueEnd = json.find_first_of(",}\n", valueStart);
		if (valueEnd == std::string::npos) {
			valueEnd = json.size();
		}
		std::string raw = json.substr(valueStart, valueEnd - valueStart);

		size_t first = raw.find_first_not_of(" \t\r\"");
		size_t last = raw.find_last_not_of(" \t\r\"");
		if (first == std::string::npos) {
			return "";
		}
		return raw.substr(first, last - first + 1);
	}
}

AnimationConfig AnimationConfig::loadFromFile(const std::string& path) {
	std::string json = readWholeFile(path);

	AnimationConfig config;

	std::string speed = extractRawValue(json, "speed_m_per_sec");
	if (!speed.empty()) {
		config.speedMetersPerSec = std::stod(speed);
	}

	std::string nextState = extractRawValue(json, "next_state_when_finished");
	if (!nextState.empty()) {
		config.nextStateWhenFinished = PieceAnimationStateNotation::parseFolderName(nextState);
	}

	std::string fps = extractRawValue(json, "frames_per_sec");
	if (!fps.empty()) {
		config.framesPerSec = std::stoi(fps);
	}

	std::string loop = extractRawValue(json, "is_loop");
	config.isLoop = (loop == "true");

	return config;
}
