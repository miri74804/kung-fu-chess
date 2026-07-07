#pragma once

#include <vector>
#include "Types.h"

class Board {
private:
	std::vector<std::vector<Token>> board;
	size_t expectedWidth;

public:
	Board();

	bool addRow(const std::vector<Token>& row);
	void print() const;
};
