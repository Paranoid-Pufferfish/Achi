//
// Created by crystal on 16/11/24.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "decision_tree.h"

bool isUsed(int used[], int size, int num) {
    for (int i = 0; i < size; ++i) {
        if (used[i] == num) {
            return true;
        }
    }
    return false;
}

tree initTree(int n, int index, int used[], int size) {
    tree currentTree = malloc(sizeof(tree_node));
    currentTree->index = index;
    used[size] = index;
    size++;

    if (n < 6) {
        int childIndex = 0;
        for (int i = 0; i < 9; ++i) {
            if (!isUsed(used, size, i)) {
                currentTree->next[childIndex] = initTree(n + 1, i, used, size);
                childIndex++;
            }
        }
    }

    return currentTree;
}