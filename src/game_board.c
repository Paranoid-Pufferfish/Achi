//
// Created by crystal on 29/11/24.
//

#include "game_board.h"

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
board create_board() {
    board game_board = calloc(9,sizeof(square));
    if (game_board == nullptr)
        exit(EXIT_FAILURE);
    int count = 0;
    for (int i = 0; i < 9; ++i) {
        game_board[i].occupied_by = 0;
    if (i != 4) {
        game_board[i].adjacent[0] = &game_board[adjacencyMatrix[i][0]];
        game_board[i].adjacent[1] = &game_board[adjacencyMatrix[i][1]];
        game_board[i].adjacent[2] = &game_board[4];
        game_board[4].adjacent[count] = &game_board[i];
        count++;
    }
    }
    return game_board;
}

void output_board(board game_board) {
    printf("Player 1 : X, Player -1 : Y\nBoard :\n");
    for (int i = 0; i < 9; ++i) {
        if (game_board[i].occupied_by == 0)
            printf(".");
        else if (game_board[i].occupied_by == 1)
            printf("X");
        else
            printf("O");

        if (i == 2 || i == 5 || i == 8)
            printf("\n");
        else
            printf(" ");
    }
}
int *get_played(board game_board, int *number, int player) {
    *number = 0;
    for (int i = 0; i < 9; ++i) {
        if (game_board[i].occupied_by == player)
            (*number)++;
    }
    if (*number == 0)
        return nullptr;
    int *empty_squares = calloc(*number,sizeof(int));
    int index = 0;
    for (int i = 0; i < 9; ++i) {
        if (game_board[i].occupied_by == player) {
            empty_squares[index] = i;
            index++;
        }
    }
    return empty_squares;
}
int *get_adjacent(board game_board, int *number, int place) {
    *number = 0;
    int *adjacents = calloc(3,sizeof(int));
        for (int i = 0; i < ((place == 4) ? 8 :3); ++i) {
            if (game_board[place].adjacent[i]->occupied_by == 0) {
                adjacents[*number] = i;
                (*number)++;
            }
        }

    return adjacents;
}

board copy_board(board game_board) {
    board new_game_board = create_board();
    for (int i = 0; i < 9; ++i) {
       new_game_board[i].occupied_by = game_board[i].occupied_by;
    }
    return new_game_board;
}
board next_board(board game_board, int placement, int round) {
    if (game_board == nullptr)
        return nullptr;
    int number_empty;
    int *empty_squares = get_played(game_board,&number_empty,0);
    bool empty = false;
    if (round <= 6) {
        for (int i = 0; i < 9; ++i) {
            if (placement == empty_squares[i]) {
                empty = true;
                break;
            }
        }
        free(empty_squares);
        if (!empty)
            return nullptr;
        int t = (round % 2 == 0) ?  -1 : 1;
        board new_game_board = copy_board(game_board);
        new_game_board[placement].occupied_by = t;
        return new_game_board;
    }
    else {
        int t = (round % 2 == 0) ?  -1 : 1;
        int number_played;
        int *player_squares = get_played(game_board,&number_played,t);
        board new_game_board = copy_board(game_board);
        int *possible_adjacents = get_adjacent(new_game_board, &number_played,player_squares[placement/3]);
        new_game_board[player_squares[placement/3]].occupied_by = 0;
        new_game_board[player_squares[placement/3]].adjacent[possible_adjacents[placement%3]]->occupied_by = t;
        return new_game_board;
    }
}
