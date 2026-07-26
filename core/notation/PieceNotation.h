#pragma once

#include "../model/Color.h"
#include "../model/PieceType.h"

class PieceNotation {
public:
	static char toSymbol(PieceType type);
	static char toSymbol(Color color);
	static PieceType parsePieceType(char c);
	static Color parseColor(char c);
};
