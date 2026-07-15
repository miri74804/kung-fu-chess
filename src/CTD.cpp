#include <iostream>
#include "io/BoardParser.h"
#include "texttests/TextTestRunner.h"

int main() {
    BoardParser boardParser;
    Board board = boardParser.parseBoard();

    TextTestRunner textTestRunner;
    textTestRunner.readCommands(board);

    return 0;
}

