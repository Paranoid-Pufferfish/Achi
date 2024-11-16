//
// Created by crystal on 16/11/24.
//

#include "decision_tree.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


tree initTree(int n, int index) {
    tree currentTree = malloc(sizeof(tree_node));
    currentTree->index = index;

    if (index == -1) {
        for (int i = 0; i < 9; ++i) {
            currentTree->next[i] = initTree(n + 1, i);
        }
    } else {
        for (int i = 0; i < 9 - n; ++i) {
            if (index > i)
                currentTree->next[i] = initTree(n + 1, i);
            else if (index < i)
                currentTree->next[i - 1] = initTree(n + 1, i);
        }
    }
    return currentTree;
}
