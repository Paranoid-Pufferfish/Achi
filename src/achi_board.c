//
// Created by crystal on 14/11/24.
//
#include "achi_board.h"

board initBoard() {
    /*
 * 0 1 2
 * 3 4 5
 * 6 7 8
    */
    board board = {0,-1};
    for (int i = 0; i < 9; ++i) {
        node node = {i,-1};
        board.nodes[i] = &node;
    }
    for (int i = 0; i < 9; ++i) {
        if (i != 4) {
            board.nodes[i]->adjacent[0] = board.nodes[adjacencyMatrix[i][0]];
            board.nodes[i]->adjacent[1] = board.nodes[adjacencyMatrix[i][1]];
            board.nodes[i]->adjacent[2] = board.nodes[4];
        }
        if(i < 4)
            board.nodes[4]->adjacent[i] = board.nodes[i];
        else if (i > 4)
            board.nodes[4]->adjacent[i-1] = board.nodes[i];
    }
    return board;
}
