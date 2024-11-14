#include <stdio.h>
#include "achi_board.h"

int main(void) {
    const board board = initBoard();

    for (int i = 0; i < 9; ++i) {
        printf("Adjacent to Node %d :{", board.nodes[i]->index);
        for (int j = 0; (j < 3 && i != 4) || (j < 8 && i == 4); ++j) {
            printf("%d", board.nodes[i]->adjacent[j]->index);
            if ((i == 4 && j != 7) || (i != 4 && j != 2))
                printf(",");
        }
        printf("}\n");
    }
    return 0;
}
