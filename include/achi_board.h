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
constexpr int adjacencyMatrix[9][2] {
    {1,3},
    {0,2},
    {1,5},
    {0,6},
    {4,4}, // Place holder
    {2,8},
    {3,7},
    {6,8},
    {7,5}
};
board initBoard();
#endif //ACHI_BOARD_H
