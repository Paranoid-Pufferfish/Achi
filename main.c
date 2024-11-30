#include <stdio.h>
#include <stdlib.h>
#include "game_board.h"

int main(void) {
    char buf[6];
    int round = 1;
    int max_round = 11  ;
    board test_board = create_board();
    board P = test_board;
    while (!is_winning(test_board) && max_round > round) {
        P = test_board;
        output_board(test_board);
        int place;
        if (round % 2 != 0) {
            if (round < 6) {
                printf("N°%d : Player 1, play your move (1-9) : ", round);
                fgets(buf, 5,stdin);
                place = strtol(buf, nullptr, 10) - 1;
            }
            else {
                output_possible(test_board,1);
                printf("N°%d : Player 1, Select the piece you want to move (1 for first piece, 2 for second...etc, from the top left) : ", round);
                fgets(buf, 5,stdin);
                place = (strtol(buf, nullptr, 10)-1) * 3;
                printf("N°%d : Player 1, Select lthe neighbour you want to move it to : (TODO: Visual indicator) ", round);
                fgets(buf, 5,stdin);
                place = place + (strtol(buf, nullptr, 10)-1);
            }
        } else {
            pair hint = minimax(test_board, false, round, max_round);
            place = hint.best_move;
            printf("N°%d : The IA played %d with an eval of %d\n",round,place+1,hint.eval);
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
    if (max_round <= round) {
        printf("Tie\n");
    }
    else {
        printf("Player %c wins !!!\n", ((is_winning(test_board) == 1) ? 'X' : 'O'));
    }
    free(test_board);
    return 0;
}