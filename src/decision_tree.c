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
    pair hint = minimax(game_board, minimizing, round, max_rounds);
    int place = hint.best_move;
    clock_t end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("N°%d : The IA played %d with an eval of %d and a time of %fs\n", round, place + 1, hint.eval, time_spent);
    return place;
}

pair minimax(board game_board, const bool maximizing, int n, int max_depth) {
    pair pair;
    if (n > max_depth) {
        pair.eval = 0;
        pair.best_move = -1;
        return pair;
    }
    if (is_winning(game_board) == 1) {
        pair.eval = 1;
        pair.best_move = -1;
        return pair;
    }
    if (is_winning(game_board) == -1) {
        pair.eval = -1;
        pair.best_move = -1;
        return pair;
    }
    if (maximizing) {
        int max_eval = -50;
        int best_move = -1;

        for (int i = 0; i < 9; ++i) {
            board next_playing_board = next_board(game_board, i, n);
            if (next_playing_board != nullptr) {
                int eval = minimax(next_playing_board, false, n + 1, max_depth).eval;
                if (eval >= max_eval) {
                    max_eval = eval;
                    best_move = i;
                }
                free(next_playing_board);
            }
        }

        pair.eval = max_eval;
        pair.best_move = best_move;
        return pair;
    } else {
        int min_eval = 5;
        int best_move = -1;
        for (int i = 0; i < 9; ++i) {
            board next_playing_board = next_board(game_board, i, n);
            if (next_playing_board != nullptr) {
                int eval = minimax(next_playing_board, true, n + 1, max_depth).eval;
                if (eval <= min_eval) {
                    min_eval = eval;
                    best_move = i;
                }
                free(next_playing_board);
            }
        }
        pair.eval = min_eval;
        pair.best_move = best_move;
        return pair;
    }
}
