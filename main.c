#include <stdio.h>

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

board initBoard() {
    board board = {0,-1};
    for (int i = 0; i < 9; ++i) {
        node node = {i,-1};
        board.nodes[i] = &node;
    }

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
    /*
     * 0 1 2
     * 3 4 5
     * 6 7 8
     */
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

int main(void) {
    printf("Hello, World!\n");
    return 0;
}
