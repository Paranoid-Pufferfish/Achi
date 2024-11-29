//
// Created by crystal on 29/11/24.
//

#ifndef GAME_BOARD_H
#define GAME_BOARD_H
typedef struct square {
    int occupied_by;
    struct square *adjacent[8];
} square;

typedef square *board;

board create_board();

void output_board(board game_board);

int *get_played(board game_board, int *number, int player);

board next_board(board game_board, int placement, int round);
#endif //GAME_BOARD_H
