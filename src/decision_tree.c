//
// Created by crystal on 29/11/24.
//
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#include "decision_tree.h"

#include <pthread.h>

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

typedef struct args_struct {
    board game_board;
    int i;
    int n;
    int max_depth;
    int *eval;
    bool maximizing;
} args_struct;

void *routine(void *args) {
    args_struct arguments = *(args_struct *) args;
    board next_playing_board = next_board(arguments.game_board, arguments.i, arguments.n);
    if (next_playing_board != nullptr) {
        *(arguments.eval) = minimax(next_playing_board, arguments.maximizing, arguments.n + 1, arguments.max_depth).eval;
        free(next_playing_board);
    }
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
                int eval = minimax(next_playing_board, true, n + 1, max_depth).eval;
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

pair minimax_mth(board game_board, const bool maximizing, int n, int max_depth, int thread_count) {
    pair Pair;
    if (maximizing) {
        pthread_t threads[thread_count];
        args_struct *arguments = calloc(9, sizeof(args_struct));
        int *evals = calloc(9, sizeof(int));
        for (int j = 0; j * thread_count < 8; ++j) {
            for (int i = 0; i < thread_count; ++i) {
                arguments[j * thread_count + i].game_board = game_board;
                arguments[j * thread_count + i].i = j * thread_count + i;
                arguments[j * thread_count + i].n = n;
                arguments[j * thread_count + i].max_depth = max_depth;
                arguments[j * thread_count + i].eval = &evals[j * thread_count + i];
                arguments[j * thread_count + i].maximizing = false;
                evals[j * thread_count + i] = -2;
                pthread_create(&threads[i], nullptr, routine, &arguments[j * thread_count + i]);
            }
            for (int i = 0; i < thread_count; ++i) {
                pthread_join(threads[i], nullptr);
            }
        }
        arguments[8].game_board = game_board;
        arguments[8].i = 8;
        arguments[8].n = n;
        arguments[8].max_depth = max_depth;
        arguments[8].eval = &evals[8];
        evals[8] = -2;
        pthread_create(&threads[0], nullptr, routine, &arguments[8]);
        pthread_join(threads[0], nullptr);
        int max_i = -1;
        int max_eval = (int) -INFINITY;
        for (int i = 0; i < 9; ++i) {
            if (evals[i] > max_eval && evals[i] != -2) {
                max_i = i;
                max_eval = evals[i];
            }
        }

        Pair.eval = evals[max_i];
        Pair.best_move = max_i;
        free(evals);
        free(arguments);
        return Pair;
    } else {
        pthread_t threads[thread_count];
        args_struct *arguments = calloc(9, sizeof(args_struct));
        int *evals = calloc(9, sizeof(int));
        for (int j = 0; j * thread_count < 8; ++j) {
            for (int i = 0; i < thread_count; ++i) {
                arguments[j * thread_count + i].game_board = game_board;
                arguments[j * thread_count + i].i = j * thread_count + i;
                arguments[j * thread_count + i].n = n;
                arguments[j * thread_count + i].max_depth = max_depth;
                arguments[j * thread_count + i].eval = &evals[j * thread_count + i];
                arguments[j * thread_count + i].maximizing = true;
                evals[j * thread_count + i] = -2;
                pthread_create(&threads[i], nullptr, routine, &arguments[j * thread_count + i]);
            }
            for (int i = 0; i < thread_count; ++i) {
                pthread_join(threads[i], nullptr);
            }
        }
        arguments[8].game_board = game_board;
        arguments[8].i = 8;
        arguments[8].n = n;
        arguments[8].max_depth = max_depth;
        arguments[8].eval = &evals[8];
        evals[8] = -2;
        pthread_create(&threads[0], nullptr, routine, &arguments[8]);
        pthread_join(threads[0], nullptr);
        int min_i = -1;
        int min_eval = (int) INFINITY;
        for (int i = 0; i < 9; ++i) {
            if (evals[i] < min_eval && evals[i] != -2) {
                min_i = i;
                min_eval = evals[i];
            }
        }

        Pair.eval = evals[min_i];
        Pair.best_move = min_i;
        free(evals);
        free(arguments);
        return Pair;
    }
}
