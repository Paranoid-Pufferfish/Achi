//
// Created by crystal on 29/11/24.
//

#ifndef DECISION_TREE_H
#define DECISION_TREE_H
#include "game_board.h"


int ai_play(board game_board, int round, int minimizing, int max_rounds);

void minimax(board game_board, const bool maximizing, int n, int max_depth, int *eval, int *best_move);
#endif //DECISION_TREE_H
