#include <stdio.h>
#include <stdlib.h>
#include "game_board.h"
#include "decision_tree.h"
#include "game_interface.h"


int main(void) {
    int round = 1;
    int turn = 1;
    int game_mode;
    int max_rounds = 6;
    bool ai_first = false;
    menu(&turn, &game_mode, &max_rounds, &ai_first);
    board game_board = create_board();
    while (!is_winning(game_board) && max_rounds > round) {
        board P = game_board;
        output_board(game_board);
        int place;
        if (game_mode == 1) {
            if (turn == 1) {
                place = player_play(game_board, round, 1);
                turn = -1;
            } else {
                place = player_play(game_board, round, -1);
                turn = 1;
            }
        } else if (game_mode == 2) {
            if (turn == 1) {
                place = player_play(game_board, round, (ai_first ? -1 : 1));
                turn = -1;
            } else {
                place = ai_play(game_board, round, ai_first, max_rounds);
                turn = 1;
            }
        } else {
            if (turn == 1) {
                place = ai_play(game_board, round, false, max_rounds);
                turn = -1;
            } else {
                place = ai_play(game_board, round, true, max_rounds);
                turn = 1;
            }
        }
        game_board = next_board(game_board, place, round);
        if (game_board == nullptr) {
            printf("There was an error, try again\n");
            game_board = P;
        } else {
            free(P);
            round++;
        }
    }
    output_board(game_board);
    if (is_winning(game_board)) {
        printf("Player %c wins !!!\n", ((is_winning(game_board) == 1) ? 'X' : 'O'));
    } else {
        printf("Tie\n");
    }
    free(game_board);

    return 0;
}
