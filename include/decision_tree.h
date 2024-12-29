//
// Created by crystal on 29/11/24.
//

#ifndef DECISION_TREE_H
#define DECISION_TREE_H
#include "game_board.h"

typedef struct pair {
    int eval;
    int best_move;
} pair;

int ai_play(board game_board, int round, int minimizing, int max_rounds);

pair minimax(board game_board, const bool maximizing, int n, int max_depth);
pair minimax_mth(board game_board, const bool maximizing, int n, int max_depth, int thread_count);
#endif //DECISION_TREE_H
