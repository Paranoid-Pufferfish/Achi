#include <stdio.h>
#include <stdlib.h>

#include "achi_board.h"
#include "decision_tree.h"

int main(void) {
    int used[9] = {-1};
    tree tree = initTree(0, -1, used, 0);
    printf("Game: %d->%d->%d->%d->%d->%d->%d->%d", tree->index,
           tree->next[0]->index,
           tree->next[0]->next[1]->index,
           tree->next[0]->next[1]->next[2]->index,
           tree->next[0]->next[1]->next[2]->next[3]->index,
           tree->next[0]->next[1]->next[2]->next[3]->next[4]->index,
           tree->next[0]->next[1]->next[2]->next[3]->next[4]->next[5]->index,
           tree->next[0]->next[1]->next[2]->next[3]->next[4]->next[5]->next[0]->index);
    return 0;
}

