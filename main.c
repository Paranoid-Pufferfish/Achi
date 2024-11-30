#include <stdio.h>
#include <stdlib.h>
#include "game_board.h"

int main(void) {
    char buf[6];
    int round = 1;
    board test_board = create_board();
    board P = test_board;
    while (!is_winning(test_board)) {
        P = test_board;
        output_board(test_board);
        int place;
        if (round % 2 != 0) {
            printf("N°%d : Player 1, play your move (0-9) : ", round);
            fgets(buf, 5,stdin);
            place = strtol(buf, nullptr, 10);
        } else {
            pair hint = minimax(test_board, false, round, 10);
            place = hint.best_move;
        }
        test_board = next_board(test_board, place, round);
        if (test_board == nullptr) {
            printf("Wtf bro, not cool :(\n");
            test_board = P;
        } else {
            free(P);
            round++;
        }
    }
    output_board(test_board);
    free(test_board);
    return 0;
}