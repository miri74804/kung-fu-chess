// Repo: https://github.com/miri74804/kung-fu-chess

#include <iostream>
#include <stdexcept>
#include "io/BoardParser.h"
#include "texttests/TextTestRunner.h"

int main() {
    BoardParser boardParser;
    Board board;

    try {
        board = boardParser.parseBoard();
    }
    catch (const std::runtime_error& e) {
        std::cout << e.what() << "\n";
        return 0;
    }

    TextTestRunner textTestRunner;
    textTestRunner.readCommands(board);

    return 0;
}

