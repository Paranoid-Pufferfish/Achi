//
// Created by crystal on 16/11/24.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "decision_tree.h"
tree_node successNode = {-2, {nullptr}};
tree_node failNode = {-3, {nullptr}};
tree_node tieNode = {-4, {nullptr}};

bool isUsed(int used[], int size, int num) {
    for (int i = 0; i < size; ++i) {
        if (used[i] == num) {
            return true;
        }
    }
    return false;
}

tree initTree(const int n, const int index, int used[], int size) {
    tree currentTree = malloc(sizeof(tree_node));
    currentTree->index = index;
    used[size] = index;
    size++;

    if (n < 6) {
        for (int i = 0; i < 9; ++i) {
            if (!isUsed(used, size, i)) {
                currentTree->next[i] = initTree(n + 1, i, used, size);
            } else {
                currentTree->next[i] = nullptr;
            }
        }
    }
    else {
        for (int i = 0; i < 9; ++i) {
            currentTree->next[0] = &tieNode;
        }
    }

    return currentTree;
}