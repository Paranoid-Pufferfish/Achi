//
// Created by crystal on 14/11/24.
//

#ifndef ACHI_BOARD_H
#define ACHI_BOARD_H

typedef struct node {
    int occupiedBy;
    int index;
    struct node *adjacent[8];
} node;

typedef struct board {
    int turn;
    node *nodes;
} board;

board initBoard();
void outputBoard(const board *playingBoard);

int playMove(board *playingBoard, int place);

int movePiece(board *const playingBoard, const int initPlace, const int finalPlace);

bool isWinningBoard(const board *playingBoard);

#endif //ACHI_BOARD_H
