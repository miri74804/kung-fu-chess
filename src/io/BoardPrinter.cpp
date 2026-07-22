#include "BoardPrinter.h"
#include <iostream>
#include "../model/Board.h"
#include "../model/Piece.h"
#include "../notation/PieceNotation.h"

void BoardPrinter::print(const Board& board) {
    for (int i = 0; i < board.getHeight(); ++i) {
        for (int j = 0; j < board.getWidth(); ++j) {
            if (j > 0) {
                std::cout << " ";
            }

            Position pos(i, j);
            const Piece* piece = board.getPieceAt(pos);

            if (piece == nullptr) {
                std::cout << ".";
            }
            else {
                std::cout << PieceNotation::toSymbol(piece->getColor());
                std::cout << PieceNotation::toSymbol(piece->getType());
            }
        }
        std::cout << "\n";
    }
}