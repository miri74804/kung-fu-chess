#pragma once

#include <string>
#include <vector>
#include <sstream>

inline void trimCarriageReturn(std::string& line) {
	if (!line.empty() && line.back() == '\r') {
		line.pop_back();
	}
}

inline std::vector<std::string> splitLine(const std::string& line) {
	std::vector<std::string> tokens;
	std::stringstream ss(line);
	std::string token;
	while (ss >> token) {
		tokens.push_back(token);
	}
	return tokens;
}

