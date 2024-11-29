#include <stdio.h>
#include <stdlib.h>
#include "game_board.h"

int main(void) {
    board test_board = create_board();
    output_board(test_board);
    test_board = next_board(test_board, 0, 1);
    output_board(test_board);
    test_board = next_board(test_board, 1, 2);
    output_board(test_board);
    test_board = next_board(test_board, 2, 3);
    output_board(test_board);
    test_board = next_board(test_board, 3, 4);
    output_board(test_board);
    test_board = next_board(test_board, 4, 5);
    output_board(test_board);
    test_board = next_board(test_board, 5, 6);
    output_board(test_board);
    test_board = next_board(test_board, 6, 7);
    output_board(test_board);
    test_board = next_board(test_board, 0, 8);
    output_board(test_board);
    return 0;
}