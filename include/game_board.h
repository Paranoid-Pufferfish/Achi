//
// Created by crystal on 29/11/24.
//

#ifndef GAME_BOARD_H
#define GAME_BOARD_H
typedef struct square {
    int occupied_by;
    int index;
    struct square *adjacent[8];
} square;

typedef struct pair {
    int eval;
    int best_move;
} pair;

typedef square *board;
board create_board();
int is_winning(board game_board);

void output_board(board game_board);

void get_played(board game_board, int *number, int player, int *empty_squares);

void get_adjacent(board game_board, int *number, int place, int *adjacents);
board next_board(board game_board, int placement, int round);

pair minimax(board game_board, const bool maximizing, int n, int max_depth);
void output_possible(board game_board, int player);
void output_adjacent(board game_board, int place, const int *squares);
#endif //GAME_BOARD_H
