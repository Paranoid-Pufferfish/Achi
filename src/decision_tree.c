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
int winningPositions[8][3] = {
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // Rows
    {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, // Columns
    {0, 4, 8}, {2, 4, 6} // Diagonals
};

void sortInsertionArray(int array[]) {
    for (int i = 1; i < 3; ++i) {
        for (int j = i; j > 0 && array[j - 1] > array[j]; --j) {
            const int temp = array[j];
            array[j] = array[j - 1];
            array[j - 1] = temp;
        }
    }
}

bool isWinning(int spots[]) {
    bool winning = false;
    sortInsertionArray(spots);
    for (int i = 0; i < 8; ++i) {
        winning = true;
        for (int j = 0; j < 3; ++j) {
            if (winningPositions[i][j] != spots[j])
                winning = false;
        }
    }
    return winning;
}

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