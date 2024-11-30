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

int is_winning(board game_board) {
    for (int i = -1; i < 2; ++i) {
        if (i != 0) {
            for (int j = 0; j < 3; ++j) {
                if (game_board[j * 3].occupied_by == i &&
                    game_board[j * 3 + 1].occupied_by == i &&
                    game_board[j * 3 + 2].occupied_by == i) {
                    return i;
                }
                if (game_board[j].occupied_by == i &&
                    game_board[j + 3].occupied_by == i &&
                    game_board[j + 6].occupied_by == i) {
                    return i;
                }
            }
            if (game_board[0].occupied_by == i &&
                game_board[4].occupied_by == i &&
                game_board[8].occupied_by == i) {
                return i;
            }
            if (game_board[2].occupied_by == i &&
                game_board[4].occupied_by == i &&
                game_board[6].occupied_by == i) {
                return i;
            }
        }
    }
    return 0;
}

board create_board() {
    board game_board = calloc(9, sizeof(square));
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

void get_played(board game_board, int *number, int player, int *empty_squares) {
    *number = 0;
    for (int i = 0; i < 9; ++i) {
        if (game_board[i].occupied_by == player)
            (*number)++;
    }
    if (*number != 0) {
        int index = 0;
        for (int i = 0; i < 9; ++i) {
            if (game_board[i].occupied_by == player) {
                empty_squares[index] = i;
                index++;
            }
        }
    }
}

void get_adjacent(board game_board, int *number, int place, int *adjacents) {
    *number = 0;
    for (int i = 0; i < ((place == 4) ? 8 : 3); ++i) {
        if (game_board[place].adjacent[i]->occupied_by == 0) {
            adjacents[*number] = i;
            (*number)++;
        }
    }
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
    int empty_squares[9];
    get_played(game_board, &number_empty, 0, empty_squares);
    bool empty = false;
    if (round <= 6) {
        for (int i = 0; i < number_empty; ++i) {
            if (placement == empty_squares[i]) {
                empty = true;
                break;
            }
        }
        if (!empty)
            return nullptr;
        int t = (round % 2 == 0) ? -1 : 1;
        board new_game_board = copy_board(game_board);
        new_game_board[placement].occupied_by = t;
        return new_game_board;
    } else {
        int t = (round % 2 == 0) ? -1 : 1;
        if (placement / 3 >= 3) {
            printf("Not cool bro :(\n");
            return nullptr;
        }
        int number_played;
        int player_squares[3] = {-1, -1, -1};
        get_played(game_board, &number_played, t, player_squares);
        if (player_squares[placement / 3] == -1)
            return nullptr;
        board new_game_board = copy_board(game_board);
        int possible_adjacents[3] = {-1, -1, -1};
        get_adjacent(new_game_board, &number_played, player_squares[placement / 3], possible_adjacents);
        if (number_played == 0) {
            free(new_game_board);
            return nullptr;
        }
        if (possible_adjacents[placement % 3] == -1) {
            free(new_game_board);
            return nullptr;
        }
        new_game_board[player_squares[placement / 3]].occupied_by = 0;
        new_game_board[player_squares[placement / 3]].adjacent[possible_adjacents[placement % 3]]->occupied_by = t;
        return new_game_board;
    }
}

pair minimax(board game_board, const bool maximizing, int n, int max_depth) {
    pair pair;
    if (is_winning(game_board) == 1) {
        pair.eval = 1;
        pair.best_move = -1;
        return pair;
    }
    if (is_winning(game_board) == -1) {
        pair.eval = -1;
        pair.best_move = -1;
        return pair;
    }
    if (n >= max_depth) {
        pair.eval = 0;
        pair.best_move = -1;
        return pair;
    }
    if (maximizing) {
        int max_eval = -100;
        int best_move = -1;

        for (int i = 0; i < 9; ++i) {
            board next_playing_board = next_board(game_board, i, n);
            if (next_playing_board != nullptr) {
                int eval = minimax(next_playing_board, false, n + 1, max_depth).eval;
                if (eval > max_eval) {
                    max_eval = eval;
                    best_move = i;
                }
                free(next_playing_board);
            }
        }

        pair.eval = max_eval;
        pair.best_move = best_move;
        return pair;
    } else {
        int min_eval = 100;
        int best_move = -1;
        for (int i = 0; i < 9; ++i) {
            board next_playing_board = next_board(game_board, i, n);
            if (next_playing_board != nullptr) {
                int eval = minimax(next_playing_board, true, n + 1, max_depth).eval;
                if (eval < min_eval) {
                    min_eval = eval;
                    best_move = i;
                }
                free(next_playing_board);
            }
        }
        pair.eval = min_eval;
        pair.best_move = best_move;
        return pair;
    }
}
