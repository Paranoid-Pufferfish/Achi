#include <stdio.h>
#include <stdlib.h>
#include "game_board.h"

int main (int argc, char *argv[]){
    char buf[6];
    int round=1;
    board test_board = create_board();
    board P = test_board;
    while (!is_winning(test_board)) {
        P = test_board;
        output_board(test_board);
        printf("N°%d : Player %d, play your move (0-9) : ", round, (round % 2 == 0) ? -1 : 1);
        pair hint = minimax(test_board,(round % 2 == 0) ? false : true,round,6);
        printf("{%d,%d}\n",hint.best_move,hint.eval);
        fgets(buf, 5,stdin);
        int place = strtol(buf, nullptr, 10);
        test_board = next_board(test_board, place, round);
        if (test_board == nullptr) {
            printf("Wtf bro, not cool :(\n");
            test_board = P;
        }
        else {
            free(P);
            round++;
        }

    }
    return 0;
}