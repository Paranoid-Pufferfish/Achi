//
// Created by crystal on 14/11/24.
//

#ifndef ACHI_BOARD_H
#define ACHI_BOARD_H
typedef struct node {
    int index;
    int occupiedBy;
    struct node *adjacent[8];
}node;
typedef struct board {
    bool win;
    int turn;
    node *nodes[9];
}board;
board initBoard();
#endif //ACHI_BOARD_H
