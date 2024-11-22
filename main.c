#include <stdio.h>
#include <stdlib.h>

#include "achi_board.h"

int main(void) {
    board playingBoard = initBoard();
    int i = 0;
    while (!isWinningBoard(&playingBoard)) {
        if (i < 6)
            printf("====Placement phase====\n");
        else
            printf("====Moving phase====\n");
        char buf[1024];
        outputBoard(&playingBoard);
        if (i < 6) {
            //NOLINTBEGIN(cppcoreguidelines-narrowing-conversions)
            printf("N°%d : Player %d, play your move (0-9) : ", i, playingBoard.turn);
            fgets(buf, 1024,stdin);
            const int pos = strtol(buf, nullptr, 10);
            if (playMove(&playingBoard, pos))
                i++;
            //NOLINTEND(cppcoreguidelines-narrowing-conversions)
        } else {
            //NOLINTBEGIN(cppcoreguidelines-narrowing-conversions)
            printf("N°%d : Player %d, Select the piece you want to move (0-9) : ", i, playingBoard.turn);
            fgets(buf, 1024,stdin);
            const int init = strtol(buf, nullptr, 10);
            printf("N°%d : Player %d, Select the place you want to move it to (0-9) : ", i, playingBoard.turn);
            fgets(buf, 1024,stdin);
            const int final = strtol(buf, nullptr, 10);
            if (movePiece(&playingBoard, init, final))
                i++;
            //NOLINTEND(cppcoreguidelines-narrowing-conversions)
        }
    }
    outputBoard(&playingBoard);
    printf("After %d moves, The player %d Won !!!", i, !playingBoard.turn);
    return 0;
}

