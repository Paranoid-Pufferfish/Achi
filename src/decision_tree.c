//
// Created by crystal on 29/11/24.
//
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "decision_tree.h"
#include "game_board.h"

int ai_play(board game_board, int round, int minimizing, int max_rounds) {
    clock_t begin = clock();
    int hint = -1;
    int eval = 0;
    minimax(game_board, minimizing, round, max_rounds, &eval,&hint);
    clock_t end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("N°%d : The IA played %d with an eval of %d and a time of %fs\n", round, hint + 1, eval, time_spent);
    return hint;
}

void minimax(board game_board, const bool maximizing, int n, int max_depth, int *eval, int *best_move){
    if (n > max_depth) {
        *eval = 0;
        return;
    }
    if (is_winning(game_board) == 1) {
        *eval = 1;
        return;
    }
    if (is_winning(game_board) == -1) {
        *eval = -1;
        return;
    }
    if (maximizing) {
        int max_eval = -50;

        for (int i = 0; i < 9; ++i) {
            board next_playing_board = next_board(game_board, i, n);
            if (next_playing_board != nullptr) {
                minimax(next_playing_board, false, n + 1, max_depth,eval,best_move);

                if (*eval >= max_eval) {
                    max_eval = *eval;
                    *best_move = i;
                }
                free(next_playing_board);
            }
        }

        *eval = max_eval;
        return;
    } else {
        int min_eval = 50;
        for (int i = 0; i < 9; ++i) {
            board next_playing_board = next_board(game_board, i, n);
            if (next_playing_board != nullptr) {
                minimax(next_playing_board, true, n + 1, max_depth,eval,best_move);

                if (*eval <= min_eval) {
                    min_eval = *eval;
                    *best_move = i;
                }
                free(next_playing_board);
            }
        }

        *eval = min_eval;
        return;
    }
}
