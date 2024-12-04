//
// Created by crystal on 29/11/24.
//

#ifndef GAME_INTERFACE_H
#define GAME_INTERFACE_H
#include "game_board.h"

int player_play(board game_board, int round, int player);

void menu(int *turn, int *game_mode, int *max_rounds, bool *ai_first);
#endif //GAME_INTERFACE_H
