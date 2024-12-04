//
// Created by crystal on 29/11/24.
//

#include <stdio.h>
#include <stdlib.h>

#include "game_interface.h"

void menu(int *turn, int *game_mode, int *max_rounds, bool *ai_first) {
    char buf[6];
    do {
        printf("Select your mode:\n1)PVP\n2)PVA\n3)AVA\nInput: ");
    } while (!fgets(buf, 5,stdin) || strtol(buf, nullptr, 10) > 3 || strtol(buf, nullptr, 10) < 1);
    *game_mode = (int) strtol(buf, nullptr, 10);
    switch (*game_mode) {
        case 1: printf("Playing against a human\n");
            break;
        case 2: printf("Playing against an AI\n");
            do {
                printf("Would you like to start first?\n1)Yes\n2)No\nInput: ");
            } while (!fgets(buf, 5,stdin) || strtol(buf, nullptr, 10) > 2 || strtol(buf, nullptr, 10) < 1);
            *turn = (strtol(buf, nullptr, 10) == 1) ? 1 : -1;
            *ai_first = (strtol(buf, nullptr, 10) == 1) ? false : true;
            printf((*turn == 1) ? "You start first\n" : "You start second\n");
            break;
        case 3: printf("Letting an AI play against an AI\n");
            break;
        default: printf("Error that should never happen, if it appears, you are cooked\n");
            break;
    }
    do {
        printf(
            "How Many rounds do you want to play? (The more rounds, the more time it will take to calculate a move. Suggested : 12):\nInput: ");
    } while (!fgets(buf, 5,stdin) || (int) strtol(buf, nullptr, 10) <= 0);
    *max_rounds = (int) strtol(buf, nullptr, 10) + 1;
}

int player_play(board game_board, int round, int player) {
    int place;
    char buf[6];
    if (round < ((player == 1) ? 6 : 7)) {
        do {
            printf("N°%d : Player %c, play your move (1-9): ", round, (player == 1) ? 'X' : 'O');
        } while (!fgets(buf, 5,stdin) || strtol(buf, nullptr, 10) > 9 || strtol(buf, nullptr, 10) < 1);
        place = (int) strtol(buf, nullptr, 10) - 1;
    } else {
        output_possible(game_board, 1);
        int number_played = 0;
        int player_squares[3] = {-1, -1, -1};
        int number_played2 = 0;
        int possible_squares[3] = {-1, -1, -1};
        get_played(game_board, &number_played, player, player_squares);
        do {
            do {
                printf("N°%d : Player %c, Select the piece you want to move: ",
                       round, (player == 1) ? 'X' : 'O');
            } while (!fgets(buf, 5,stdin) || strtol(buf, nullptr, 10) > 3 || strtol(buf, nullptr, 10) < 1);
            place = (int) (strtol(buf, nullptr, 10) - 1) * 3;
            get_adjacent(game_board, &number_played2, player_squares[place / 3], possible_squares);
            if (number_played2 <= 0)
                printf("No adjacent squares found\n");
        } while (number_played2 <= 0);
        output_adjacent(game_board, player_squares[place / 3], possible_squares);
        do {
            printf("N°%d : Player %c, Select the neighbour you want to move it to: ",
                   round, (player == 1) ? 'X' : 'O');
        } while (!fgets(buf, 5,stdin));
        place = place + (int) (strtol(buf, nullptr, 10) - 1);
    }
    return place;
}
