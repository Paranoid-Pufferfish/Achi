//
// Created by crystal on 14/11/24.
//
#include "achi_board.h"

#include <stdlib.h>
const int adjacencyMatrix[9][2] = {
    {1, 3},
    {0, 2},
    {1, 5},
    {0, 6},
    {4, 4}, // Place holder
    {2, 8},
    {3, 7},
    {6, 8},
    {7, 5}
};
/*
  0 1 2
  3 4 5
  6 7 8
*/
board initBoard() {
    board board;
    board.win = 0;
    board.turn = -1;
    for (int i = 0; i < 9; ++i) {
        node *tempnode = calloc(1, sizeof(node));
        tempnode->index = i;
        tempnode->occupiedBy = -1;
        board.nodes[i] = tempnode;
    }
    for (int i = 0; i < 9; ++i) {
        if (i != 4) {
            board.nodes[i]->adjacent[0] = board.nodes[adjacencyMatrix[i][0]];
            board.nodes[i]->adjacent[1] = board.nodes[adjacencyMatrix[i][1]];
            board.nodes[i]->adjacent[2] = board.nodes[4];
            if (i < 4)
                board.nodes[4]->adjacent[i] = board.nodes[i];
            else
                board.nodes[4]->adjacent[i - 1] = board.nodes[i];
        }
    }
    return board;
}
