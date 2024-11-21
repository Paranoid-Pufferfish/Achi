#include <stdio.h>
#include <stdlib.h>

#include "achi_board.h"

int main(void) {
    board playingBoard = initBoard();
    while (!isWinningBoard(&playingBoard)) {
        char buf[1024];
        outputBoard(&playingBoard);
        printf("Player %d, play your move : ",playingBoard.turn);
        fgets(buf,1024,stdin);
        const int pos = (int) strtol(buf,nullptr,10);
        playMove(&playingBoard,pos);
    }
        printf("Congrats Player %d",!playingBoard.turn);
    return 0;
}

