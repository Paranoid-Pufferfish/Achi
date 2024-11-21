#include <stdio.h>
#include <stdlib.h>

#include "achi_board.h"

int main(void) {
    board playingBoard = initBoard();
    int i = 0;
    while (!isWinningBoard(&playingBoard)) {
        char buf[1024];
        outputBoard(&playingBoard);
        if (i < 6) {
            printf("%d : Player %d, play your move : ", i, playingBoard.turn);
            fgets(buf, 1024,stdin);
            const int pos = (int) strtol(buf, nullptr, 10);
            if (playMove(&playingBoard, pos))
                i++;
        } else {
            printf("%d : Player %d, Select the piece you want to move : ", i, playingBoard.turn);
            fgets(buf, 1024,stdin);
            const int init = (int) strtol(buf, nullptr, 10);
            printf("%d : Player %d, Select the place you want to move it to : ", i, playingBoard.turn);
            fgets(buf, 1024,stdin);
            const int final = (int) strtol(buf, nullptr, 10);
            if (movePiece(&playingBoard, init, final))
                i++;
        }
    }
    outputBoard(&playingBoard);
    printf("After %d moves, The placer %d Won !!!", i, !playingBoard.turn);
    return 0;
}

