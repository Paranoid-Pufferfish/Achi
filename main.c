#include <stdio.h>
#include <stdlib.h>
#include "achi_board.h"

typedef struct tree {
    int state[9];
    struct tree *next[9];
} tree;
tree *nextPlacement(const tree *previous, const int place, const int turn) {
    tree *P = malloc(sizeof(tree));
    for (int i = 0; i < 9; ++i) {
        P->state[i] = previous->state[i];
    }
    P->state[place] = turn;
    return P;
}

tree *nextMove(const tree *previous, const int initPlace, const int finalPlace, const int turn) {
    tree *P = malloc(sizeof(tree));
    for (int i = 0; i < 9; ++i) {
        P->state[i] = previous->state[i];
    }
    P->state[initPlace] = 0;
    P->state[finalPlace] = turn;
    return P;
}

tree *initTree(const tree *previous, const int place, const int turn, const int n, const int depth) {
    if (n > depth)
        return nullptr;
    if (previous->state[place] != 0)
        return nullptr;
    tree *P = nextPlacement(previous, place, turn);
    int t;
    for (int i = 0; i < 9; ++i) {
        if (turn == 1)
            t = -1;
        else
            t = 1;
        P->next[i] = initTree(P, i, t, n + 1, depth);
    }
    return P;
}

void outputTree(const tree *T) {
    for (int i = 0; i < 9; ++i) {
        if (T->state[i] == 0)
            printf(".");
        else if (T->state[i] == 1)
            printf("X");
        else
            printf("O");

        if (i == 2 || i == 5 || i == 8)
            printf("\n");
        else
            printf(" ");
    }
}

tree makeTree() {
    tree Tree;
    for (int i = 0; i < 9; ++i) {
        Tree.state[i] = 0;
        Tree.next[i] = nullptr;
    }
    for (int i = 0; i < 9; ++i) {
        Tree.next[i] = initTree(&Tree, i, 1, 1, 6);
    }
    return Tree;
}

int main(void) {
    const tree T = makeTree();
    const tree *P = &T;
    board playingBoard = initBoard();
    int i = 1;
    while (!isWinningBoard(&playingBoard) && i < 13) {
        if (i <= 6)
            printf("====Placement phase====\n");
        else
            printf("====Moving phase====\n");
        char buf[1024];
        outputTree(P);
        if (i <= 6) {
            //NOLINTBEGIN(cppcoreguidelines-narrowing-conversions)
            printf("N°%d : Player %d, play your move (0-9) : ", i, playingBoard.turn);
            fgets(buf, 1024,stdin);
            const int pos = strtol(buf, nullptr, 10);
            if (playMove(&playingBoard, pos)) {
                P = P->next[pos];
                i++;
            }
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
    if (i >= 13)
        printf("Tie");
    else
        printf("After %d moves, The player %d lost !!!", i, playingBoard.turn);

    free(playingBoard.nodes);
    return 0;
}

