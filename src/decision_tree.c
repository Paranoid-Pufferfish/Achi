//
// Created by crystal on 29/11/24.
//
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "decision_tree.h"
#include "game_board.h"

int ai_play(board game_board, int round, int minimizing, int max_rounds) {
    clock_t begin = clock();
    pair hint = minimax(game_board, minimizing, round, max_rounds);
    int place = hint.best_move;
    clock_t end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("N°%d : The IA played %d with an eval of %d and a time of %fs\n", round, place + 1, hint.eval, time_spent);
    return place;
}

pair minimax(board game_board, const bool maximizing, int n, int max_depth) {
    pair Pair;
    int win_status = is_winning(game_board);

    if (n > max_depth) {
        Pair.eval = 0;
        Pair.best_move = -1;
        return Pair;
    }
    if (win_status != 0) {
        Pair = (win_status == 1) ? (struct pair){1, -1} : (struct pair){-1, -1};
        return Pair;
    }
    if (maximizing) {
        int max_eval = (int) -INFINITY;
        int best_move = -1;

        for (int i = 0; i < 9; ++i) {
            board next_playing_board = next_board(game_board, i, n);
            if (next_playing_board != nullptr) {
                int eval = minimax(next_playing_board, false, n + 1, max_depth).eval;
                if (eval > max_eval) {
                    max_eval = eval;
                    best_move = i;
                }
                free(next_playing_board);
            }
        }

        Pair.eval = max_eval;
        Pair.best_move = best_move;
        return Pair;
    } else {
        int min_eval = (int) INFINITY;
        int best_move = -1;
        for (int i = 0; i < 9; ++i) {
            board next_playing_board = next_board(game_board, i, n);
            if (next_playing_board != nullptr) {
                int eval = minimax(next_playing_board, true, n+1, max_depth).eval;
                if (eval < min_eval) {
                    min_eval = eval;
                    best_move = i;
                }
                free(next_playing_board);
            }
        }
        Pair.eval = min_eval;
        Pair.best_move = best_move;
        return Pair;
    }
}
