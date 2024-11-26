#include <stdio.h>
#include <stdlib.h>
#include "achi_board.h"
typedef struct tree {
    int state[9];
    struct tree *next[9];
} tree;
typedef struct pair {
    int eval;
    int best_move;
} pair;

int isTerminal(const tree *tree) {
    for (int i = -1; i < 2; ++i) {
        if (i != 0) {
            for (int j = 0; j < 3; ++j) {
                if (tree->state[j * 3] == i &&
                    tree->state[j * 3 + 1] == i &&
                    tree->state[j * 3 + 2] == i) {
                    return i;
                }
                if (tree->state[j] == i &&
                    tree->state[j + 3] == i &&
                    tree->state[j + 6] == i) {
                    return i;
                }
            }
            if (tree->state[0] == i &&
                tree->state[4] == i &&
                tree->state[8] == i) {
                return i;
            }
            if (tree->state[2] == i &&
                tree->state[4] == i &&
                tree->state[6] == i) {
                return i;
            }
        }
    }
    return 0;
}

bool isFinal(tree *tree) {
    for (int i = 0; i < 9; ++i) {
        if (tree->next[i] != nullptr)
            return false;
    }
    return true;
}
tree *nextPlacement(const tree *previous, const int place, const int turn) {
    if (previous->state[place] != 0 || isTerminal(previous) != 0)
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
        if (n < depth)
            P->next[i] = initTree(P, i, t, n + 1, depth);
    }
    return P;
}

void outputTree(const tree *T) {
    for (int i = 0; i < 9; ++i) {
        if (T->state[i] == 0)
            printf(".");
        else if (T->state[i] == 1)
            printf(PLAYER1CHAR);
        else
            printf(PLAYER2CHAR);

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
        //     printf("Winner : %d Terminal Board detected :\n", isTerminal(P));
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
            printf(PLAYER1CHAR);
            if (place == i)
                printf("\033[0m");
        } else {
            if (place == i)
                printf("\033[0;36m");
            printf(PLAYER2CHAR);
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

pair minimax(tree *board, bool maximizing) {
    pair pair;
    if (isTerminal(board) == 1) {
        pair.eval = 1;
        pair.best_move = -1;
        return pair;
    }
    if (isTerminal(board) == -1) {
        pair.eval = -1;
        pair.best_move = -1;
        return pair;
    }
    if (isFinal(board)) {
        pair.eval = 0;
        pair.best_move = -1;
        return pair;
    }
    if (maximizing) {
        int max_eval = -100;
        int best_move = -1;
        for (int i = 0; i < 9; ++i) {
            if (board->next[i] != nullptr) {
                int eval = minimax(board->next[i], false).eval;
                if (eval > max_eval) {
                    max_eval = eval;
                    best_move = i;
                }
            }
        }
        pair.eval = max_eval;
        pair.best_move = best_move;
        return pair;
    } else {
        int min_eval = 100;
        int best_move = -1;
        for (int i = 0; i < 9; ++i) {
            if (board->next[i] != nullptr) {
                int eval = minimax(board->next[i], true).eval;
                if (eval < min_eval) {
                    min_eval = eval;
                    best_move = i;
                }
            }
        }
        pair.eval = min_eval;
        pair.best_move = best_move;
        return pair;
    }
}

int main(void) {
    tree *T = makeTree();
    tree *P = T;
    board playingBoard = initBoard();
    char buf[1024];
    int i = 1;
    int pos;
    int startFirst = 0;
    printf("Would you want to play VS an AI ? [1 for yes, 0 for no]: ");
    fgets(buf, 1024,stdin);
    bool ai = strtol(buf, nullptr, 10);
    if (ai)
        printf("Player VS AI\n");
    else
        printf("Player VS Player\n");
    printf("Would you want to start first ? [1 for yes, 0 for no]: ");
    fgets(buf, 1024,stdin);
    startFirst = !strtol(buf, nullptr, 10);
    if (startFirst)
        printf("AI plays first\n");
    else
        printf("You play first\n");
    while (!isWinningBoard(&playingBoard) && i < 13) {
        if (i <= 6)
            printf("====Placement phase====\n");
        else
            printf("====Moving phase====\n");
        if (i <= 6) {
            outputTree(P);
            // outputBoard(&playingBoard);
            //NOLINTBEGIN(cppcoreguidelines-narrowing-conversions)
            if (ai) {
                if (i % 2 == startFirst)
                    pos = minimax(P, startFirst).best_move;
                else {
                    printf("N°%d : Player %d, play your move (0-9) : ", i, playingBoard.turn);
                    fgets(buf, 1024,stdin);
                    pos = strtol(buf, nullptr, 10);
                }
            } else {
                printf("N°%d : Player %d, play your move (0-9) : ", i, playingBoard.turn);
                fgets(buf, 1024,stdin);
                pos = strtol(buf, nullptr, 10);
            }
            if (playMove(&playingBoard, pos)) {
                P = P->next[pos];
                i++;
            }
            if (i == 6)
                freeAll(P);
            //NOLINTEND(cppcoreguidelines-narrowing-conversions)
        } else {
            int num[3];
            int count = 0;
            outputBoard(&playingBoard);
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
    return 0;
}

