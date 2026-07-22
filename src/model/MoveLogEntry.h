#pragma once

#include "Color.h"
#include <string>

struct MoveLogEntry {
	Color color;
	std::string notation; // e.g. "e4", "Nc6", "Bxc6", "exd4"
	int elapsedMs;         // game clock at the moment this move landed
};
