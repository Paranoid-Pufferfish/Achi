#include <stdio.h>
#include <stdlib.h>
#include "game_board.h"

int main(void) {
    int round = 1;
    int max_round = 15;
    board test_board = create_board();
    board P = test_board;
    while (!is_winning(test_board) && max_round > round) {
        P = test_board;
        output_board(test_board);
        int place;
        if (round % 2 != 0) {
            char buf[6];
            if (round < 6) {
                printf("N°%d : Player 1, play your move (1-9) : ", round);
                if (fgets(buf, 5,stdin) == NULL)
                    return 1;
                place = (int) strtol(buf, nullptr, 10) - 1;
            } else {
                output_possible(test_board, 1);
                int number_played = 0;
                int player_squares[3] = {-1, -1, -1};
                int number_played2 = 0;
                int possible_squares[3] = {-1, -1, -1};
                get_played(test_board, &number_played, 1, player_squares);
                do {
                    printf("N°%d : Player 1, Select the piece you want to move : ",
                           round);
                    if (fgets(buf, 5,stdin) == NULL)
                        return 1;
                    place = (int) (strtol(buf, nullptr, 10) - 1) * 3;
                    get_adjacent(test_board, &number_played2, player_squares[place / 3], possible_squares);
                    if (number_played2 <= 0)
                        printf("No adjacent squares found\n");
                } while (number_played2 <= 0);
                output_adjacent(test_board, player_squares[place / 3], possible_squares);
                printf("N°%d : Player 1, Select the neighbour you want to move it to : (TODO: Visual indicator) ",
                       round);
                if (fgets(buf, 5,stdin) == NULL)
                    return 1;
                place = place + (int) (strtol(buf, nullptr, 10) - 1);
            }
        } else {
            pair hint = minimax(test_board, false, round, max_round);
            place = hint.best_move;
            printf("N°%d : The IA played %d with an eval of %d\n", round, place + 1, hint.eval);
        }
        test_board = next_board(test_board, place, round);
        if (test_board == nullptr) {
            printf("Wtf bro, not cool :(\n");
            test_board = P;
        } else {
            free(P);
            round++;
        }
    }
    output_board(test_board);
    if (is_winning(test_board)) {
        printf("Player %c wins !!!\n", ((is_winning(test_board) == 1) ? 'X' : 'O'));
    } else {
        printf("Tie\n");
    }
    free(test_board);
    return 0;
}
