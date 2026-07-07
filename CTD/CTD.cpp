#include <iostream>
#include "BoardParser.h"

int main() {
    BoardParser parser;
    Board board = parser.parseInput();
    board.print();
    return 0;
}

