//
// Created by crystal on 14/11/24.
//
#include "achi_board.h"

#include <stdio.h>
#include <stdlib.h>
const int adjacencyMatrix[9][2] = {
    {1, 3},
    {0, 2},
    {1, 5},
    {0, 6},
    {4, 4}, // Place holder
    {2, 8},
    {3, 7},
    {6, 8},
    {7, 5}
};
/*
  0 1 2
  3 4 5
  6 7 8
*/
board initBoard() {
    board board;
    board.turn = 0;
    board.nodes = calloc(9, sizeof(node));
    if (board.nodes == NULL)
        exit(EXIT_FAILURE);
    for (int i = 0; i < 9; ++i) {
        board.nodes[i].index = i;
        board.nodes[i].occupiedBy = -1;
    }
    for (int i = 0; i < 9; ++i) {
        if (i != 4) {
            board.nodes[i].adjacent[0] = &board.nodes[adjacencyMatrix[i][0]];
            board.nodes[i].adjacent[1] = &board.nodes[adjacencyMatrix[i][1]];
            board.nodes[i].adjacent[2] = &board.nodes[4];
            if (i < 4)
                board.nodes[4].adjacent[i] = &board.nodes[i];
            else
                board.nodes[4].adjacent[i - 1] = &board.nodes[i];
        }
    }
    return board;
}

void outputBoard(const board *playingBoard) {
    for (int i = 0; i < 9; ++i) {
        if (playingBoard->nodes[i].occupiedBy == -1)
            printf(".");
        else if (playingBoard->nodes[i].occupiedBy == 0)
            printf("X");
        else
            printf("O");

        if (i == 2 || i == 5 || i == 8)
            printf("\n");
        else
            printf(" ");
    }
}

int playMove(board *const playingBoard, const int place) {
    if (playingBoard->nodes[place].occupiedBy != -1) {
        printf("Illegal move!!!\n");
        return 0;
    }
    if (playingBoard->turn == 1) {
        printf("Placing Y in %d\n", place);
        playingBoard->nodes[place].occupiedBy = 1;
        playingBoard->turn = 0;
        return 1;
    }
    printf("Placing X in %d\n", place);
    playingBoard->nodes[place].occupiedBy = 0;
    playingBoard->turn = 1;
    return 1;
}
int movePiece(board *const playingBoard, const int initPlace, const int finalPlace) {
    if (initPlace == finalPlace) {
        printf("Cant keep piece in one place!\n");
        return 0;
    }
    if (playingBoard->turn != playingBoard->nodes[initPlace].occupiedBy) {
        printf("Not your piece !\n");
        return 0;
    }
    if (playingBoard->nodes[finalPlace].occupiedBy != -1) {
        printf("Place occupied!\n");
        return 0;
    }
    if (initPlace == 4) {
        for (int i = 0; i < 8; ++i) {
            if (playingBoard->nodes[initPlace].adjacent[i]->index == finalPlace) {
                if (playingBoard->nodes[initPlace].adjacent[i]->occupiedBy != -1) {
                    printf("Place Occupied\n");
                    return 0;
                }
                playingBoard->nodes[initPlace].occupiedBy = -1;
                playingBoard->nodes[finalPlace].occupiedBy = playingBoard->turn;
                printf("Player %d Moved Piece from %d to %d\n", playingBoard->turn, initPlace, finalPlace);
                if (playingBoard->turn == 0)
                    playingBoard->turn = 1;
                else if (playingBoard->turn == 1)
                    playingBoard->turn = 0;
                return 1;
            }
        }
    } else {
        for (int i = 0; i < 3; ++i) {
            if (playingBoard->nodes[initPlace].adjacent[i]->index == finalPlace) {
                if (playingBoard->nodes[initPlace].adjacent[i]->occupiedBy != -1) {
                    printf("Place Occupied\n");
                    return 0;
                }
                playingBoard->nodes[initPlace].occupiedBy = -1;
                playingBoard->nodes[finalPlace].occupiedBy = playingBoard->turn;
                printf("Player %d Moved Piece from %d to %d\n", playingBoard->turn, initPlace, finalPlace);
                if (playingBoard->turn == 0)
                    playingBoard->turn = 1;
                else if (playingBoard->turn == 1)
                    playingBoard->turn = 0;
                return 1;
            }
        }
    }
    return 1;
}

bool isWinningBoard(board const *playingBoard) {
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (playingBoard->nodes[j*3].occupiedBy == i &&
                playingBoard->nodes[j * 3 + 1].occupiedBy == i &&
                playingBoard->nodes[j * 3 + 2].occupiedBy == i) {
                return true;
            }
            if (playingBoard->nodes[j].occupiedBy == i &&
                playingBoard->nodes[j + 3].occupiedBy == i &&
                playingBoard->nodes[j + 6].occupiedBy == i) {
                return true;
            }
        }
        if (playingBoard->nodes[0].occupiedBy == i &&
            playingBoard->nodes[4].occupiedBy == i &&
            playingBoard->nodes[8].occupiedBy == i) {
            return true;
        }
        if (playingBoard->nodes[2].occupiedBy == i &&
            playingBoard->nodes[4].occupiedBy == i &&
            playingBoard->nodes[6].occupiedBy == i) {
            return true;
        }
    }
    return false;
}

