#include <stdio.h>
#include <stdlib.h>

#include "achi_board.h"
#include "decision_tree.h"

int main(void) {
    int used[9] = {-1};
    tree tree = initTree(0, -1, used, 0);
    printf("Game: %d->%d->%d->%d->%d->%d->%d", tree->index,
           tree->next[0]->index,
           tree->next[0]->next[0]->index,
           tree->next[0]->next[0]->next[0]->index,
           tree->next[0]->next[0]->next[0]->next[0]->index,
           tree->next[0]->next[0]->next[0]->next[0]->next[0]->index,
           tree->next[0]->next[0]->next[0]->next[0]->next[0]->next[0]->index);
    return 0;
}

