#include <stdio.h>
#include <stdlib.h>

#include "achi_board.h"

int main(void) {
    board playingBoard = initBoard();
    playMove(&playingBoard, 0);
    playMove(&playingBoard, 5);
    playMove(&playingBoard, 4);
    playMove(&playingBoard, 6);
    playMove(&playingBoard, 8);
    printf("Is Winning %d\n", isWinningBoard(&playingBoard));
    outputBoard(&playingBoard);
    return 0;
}

