//
// Created by crystal on 16/11/24.
//

#ifndef DECISION_TREE_H
#define DECISION_TREE_H

typedef struct tree_node {
    int index;
    int rank;
    struct tree_node *next[9];
} tree_node;

typedef tree_node *tree;

tree initTree(int n, int index);
#endif //DECISION_TREE_H
