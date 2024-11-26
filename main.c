#include <stdio.h>
#include <stdlib.h>
#include "achi_board.h"

typedef struct tree {
    int state[9];
    struct tree *next[9];
} tree;

bool isTerminal(const tree *tree) {
    for (int i = -1; i < 2; ++i) {
        if (i != 0) {
            for (int j = 0; j < 3; ++j) {
                if (tree->state[j * 3] == i &&
                    tree->state[j * 3 + 1] == i &&
                    tree->state[j * 3 + 2] == i) {
                    return true;
                }
                if (tree->state[j] == i &&
                    tree->state[j + 3] == i &&
                    tree->state[j + 6] == i) {
                    return true;
                }
            }
            if (tree->state[0] == i &&
                tree->state[4] == i &&
                tree->state[8] == i) {
                return true;
            }
            if (tree->state[2] == i &&
                tree->state[4] == i &&
                tree->state[6] == i) {
                return true;
            }
        }
    }
    return false;
}

tree *nextPlacement(const tree *previous, const int place, const int turn) {
    if (previous->state[place] != 0 || isTerminal(previous))
        return nullptr;
    tree *P = malloc(sizeof(tree));
    for (int i = 0; i < 9; ++i) {
        P->state[i] = previous->state[i];
    }
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 10; ++j) {
            P->next[i] = nullptr;
        }
    }
    P->state[place] = turn;
    return P;
}

// tree *nextMove(const tree *previous, const int initPlace, const int finalPlace, const int turn) {
//     if (previous->state[initPlace] != turn || previous->state[finalPlace] != 0 || isTerminal(previous))
//         return nullptr;
//     tree *P = malloc(sizeof(tree));
//     for (int i = 0; i < 9; ++i) {
//         P->state[i] = previous->state[i];
//     }
//     P->state[initPlace] = 0;
//     P->state[finalPlace] = turn;
//     return P;
// }
// tree *initTree2(const tree *previous, const int initPlace, const int finalPlace, const int turn, const int n, const int depth) {
//     tree *P = nextMove(previous, initPlace,finalPlace, turn);
//     if (n > depth || P == nullptr)
//         return nullptr;
//     int t;
//     for (int i = 0; i < 9; ++i) {
//         for (int j = 0; j < 10; ++j) {
//             if (turn == 1)
//                 t = -1;
//             else
//                 t = 1;
//             P->next[i] = initTree2(P, i,j, t, n + 1, depth);
//         }
//     }
//     return P;
// }
tree *initTree(const tree *previous, const int place, const int turn, const int n, const int depth) {
    tree *P = nextPlacement(previous, place, turn);
    if (n > depth || P == nullptr)
        return nullptr;
    int t;
    for (int i = 0; i < 9; ++i) {
        if (turn == 1)
            t = -1;
        else
            t = 1;
        if (n < 6)
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

tree *makeTree() {
    tree *Tree = malloc(sizeof(tree));
    for (int i = 0; i < 9; ++i) {
        Tree->state[i] = 0;
        Tree->next[i] = nullptr;
    }
    for (int i = 0; i < 9; ++i) {
        Tree->next[i] = initTree(Tree, i, 1, 1, 6);
    }
    return Tree;
}
void freeAll(tree *P) {
    if (P != nullptr) {
        for (int i = 0; i < 9; ++i) {
            freeAll(P->next[i]);
        }
        free(P);
        // if (isTerminal(P)) {
        //     printf("\n=============\n");
        //     *n = *n +1;
        //     printf("%d Terminal Board detected :\n", *n);
        //     outputTree(P);
        //     printf("\n=============\n");
        // }
    }
}

bool outputPossibleMove(board *B, int place, int *num, int count) {
    if (B->nodes[place].occupiedBy != B->turn) {
        printf("Not your piece!!!\n");
        return false;
    }
    if (place > 8 || place < 0) {
        printf("Not a Valid piece!!!\n");
        return false;
    }
    count = 0;
    if (place == 4) {
        for (int i = 0; i < 8; ++i) {
            if (B->nodes[place].adjacent[i]->occupiedBy == 0) {
                num[count] = i;
                count++;
            }
        }
    } else {
        for (int i = 0; i < 3; ++i) {
            if (B->nodes[place].adjacent[i]->occupiedBy == 0) {
                num[count] = i;
                count++;
            }
        }
    }
    for (int i = 0; i < 9; ++i) {
        if (B->nodes[i].occupiedBy == 0) {
            int t = true;
            for (int j = 0; j < count; ++j) {
                if (B->nodes[place].adjacent[num[j]]->index == i) {
                    printf("\033[0;35m%d\033[0m", j);
                    t = false;
                }
            }
            if (t)
                printf(".");
        } else if (B->nodes[i].occupiedBy == 1) {
            if (place == i)
                printf("\033[0;36m");
            printf("X");
            if (place == i)
                printf("\033[0m");
        } else {
            if (place == i)
                printf("\033[0;36m");
            printf("O");
            if (place == i)
                printf("\033[0m");
        }

        if (i == 2 || i == 5 || i == 8)
            printf("\n");
        else
            printf(" ");
    }
    return true;
}

int main(void) {
    // tree *T = makeTree();
    // tree *P = T;
    board playingBoard = initBoard();
    int i = 1;
    int num[3];
    int count = 0;
    while (!isWinningBoard(&playingBoard) && i < 13) {
        if (i <= 6)
            printf("====Placement phase====\n");
        else
            printf("====Moving phase====\n");
        char buf[1024];
        // outputTree(P);
        outputBoard(&playingBoard);
        if (i <= 6) {
            //NOLINTBEGIN(cppcoreguidelines-narrowing-conversions)
            printf("N°%d : Player %d, play your move (0-9) : ", i, playingBoard.turn);
            fgets(buf, 1024,stdin);
            const int pos = strtol(buf, nullptr, 10);
            if (playMove(&playingBoard, pos)) {
                // P = P->next[pos];
                i++;
            }
            //NOLINTEND(cppcoreguidelines-narrowing-conversions)
        } else {
            //NOLINTBEGIN(cppcoreguidelines-narrowing-conversions)
            printf("N°%d : Player %d, Select the piece you want to move (0-9) : ", i, playingBoard.turn);
            fgets(buf, 1024,stdin);
            const int init = strtol(buf, nullptr, 10);
            if (outputPossibleMove(&playingBoard, init, num, count)) {
                printf("N°%d : Player %d, Select the place you want to move it to : ", i, playingBoard.turn);
                fgets(buf, 1024,stdin);
                const int final = strtol(buf, nullptr, 10);
                if (movePiece(&playingBoard, init, playingBoard.nodes[init].adjacent[num[final]]->index))
                    i++;
            }
            //NOLINTEND(cppcoreguidelines-narrowing-conversions)
        }
    }
    outputBoard(&playingBoard);
    if (i >= 13)
        printf("Tie");
    else
        printf("After %d moves, The player %d lost !!!", i, playingBoard.turn);

    free(playingBoard.nodes);
    // freeAll(P);
    return 0;
}

