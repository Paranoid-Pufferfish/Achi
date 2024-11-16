#include <stdio.h>
#include <stdlib.h>

#include "achi_board.h"
#include "decision_tree.h"

int main(void) {
    /*const board board = initBoard();

    for (int i = 0; i < 9; ++i) {
        printf("Adjacent to Node %d :{", board.nodes[i].index);
        for (int j = 0; (j < 3 && i != 4) || (j < 8 && i == 4); ++j) {
            printf("%d", board.nodes[i].adjacent[j]->index);
            if ((i == 4 && j != 7) || (i != 4 && j != 2))
                printf(",");
        }
        printf("}\n");
    }
    free(board.nodes);*/
    tree tree = initTree(0, -1);
    printf("Parent: %d \nKids: ", tree->index);
    for (int i = 0; i < 9; ++i) {
        printf("%d ", tree->next[i]->index);
    }
    printf("\nGrandKids from %d: ", tree->next[3]->index);
    for (int i = 0; i < 7; ++i) {
        printf("%d ", tree->next[3]->next[i]->index);
    }
    return 0;
}
