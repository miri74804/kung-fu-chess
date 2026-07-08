#include <iostream>
#include "BoardParser.h"
#include "CommandParser.h"

int main() {
    BoardParser boardParser;
    Board board = boardParser.parseBoard();

    CommandParser commandParser;
    commandParser.readCommands(board);

    return 0;
}

