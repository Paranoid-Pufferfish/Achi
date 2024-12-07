#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "game_board.h"
#include "decision_tree.h"
#include "game_interface.h"

#define SCREEN_WIDTH 1360
#define SCREEN_HEIGHT 740
#define BOARD_DIMS 500

typedef enum achi_scene {
    ACHI_MENU,
    ACHI_GAME_PVA,
    ACHI_GAME_PVP,
    ACHI_GAME_AVA,
    ACHI_END,
} achi_scene;

void achi_menu(SDL_Renderer *renderer, TTF_Font *font, TTF_TextEngine *text_engine, SDL_Cursor *pointing_cursor,
               bool *quit) {
    SDL_Event event;
    TTF_Text *welcome_text = TTF_CreateText(text_engine, font, "Welcome to the Achi game!", 0);
    TTF_Text *PVP_text = TTF_CreateText(text_engine, font, "1) Player VS Player", 0);
    TTF_Text *PVA_text = TTF_CreateText(text_engine, font, "2) Player VS AI", 0);
    TTF_Text *AVA_text = TTF_CreateText(text_engine, font, "3) AI VS AI", 0);
    TTF_Text *QUIT_text = TTF_CreateText(text_engine, font, "4) Quit to desktop", 0);
    TTF_SetTextColorFloat(QUIT_text, 0xFF, 0x00, 0x00,SDL_ALPHA_OPAQUE_FLOAT);
    float x_pos = 0;
    float y_pos = 0;
    SDL_GetMouseState(&x_pos, &y_pos);
    SDL_FPoint mouse = {x_pos, y_pos};
    int text_w = 0;
    int text_h = 0;
    TTF_GetTextSize(welcome_text, &text_w, &text_h);
    TTF_DrawRendererText(welcome_text, (float) (SCREEN_WIDTH - text_w) / 2, 0);
    TTF_DrawRendererText(PVP_text, 0, 200);
    TTF_DrawRendererText(PVA_text, 0, 300);
    TTF_DrawRendererText(AVA_text, 0, 400);
    TTF_DrawRendererText(QUIT_text, 0, 500);
    TTF_GetTextSize(QUIT_text, &text_w, &text_h);
    SDL_FRect QUIT_rect = {0, 500, (float) text_w, (float) text_h};
    if (SDL_PointInRectFloat(&mouse, &QUIT_rect))
        SDL_SetCursor(pointing_cursor);
    if (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT: *quit = true;
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (event.button.button == SDL_BUTTON_LEFT && SDL_PointInRectFloat(&mouse, &QUIT_rect))
                    *quit = true;
            default:
                break;
        }
    }

    SDL_RenderPresent(renderer);
}

int main(void) {
    // SDL_Window *window;
    // SDL_Renderer *renderer;
    // if (!SDL_Init(!SDL_INIT_VIDEO) || !TTF_Init()) {
    //     SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Error initializing SDL : %s\n", SDL_GetError());
    //     return 1;
    // }
    // if (!SDL_CreateWindowAndRenderer("Achi Game", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &window,
    //                                  &renderer)) {
    //     SDL_LogCritical(SDL_LOG_CATEGORY_RENDER, "Error Creating Window and Renderer : %s\n", SDL_GetError());
    //     return 1;
    // }
    //
    // achi_scene scene = ACHI_MENU;
    // bool quit = false;
    // TTF_Font *font = TTF_OpenFont("../media/Acme 9 Regular.ttf", 30);
    // TTF_TextEngine *text_engine = TTF_CreateRendererTextEngine(renderer);
    // SDL_Cursor *pointing = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
    // SDL_Cursor *default_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
    // while (!quit) {
    //     SDL_SetCursor(default_cursor);
    //     achi_menu(renderer, font, text_engine, pointing, &quit);
    // }
    int round = 1;
    int turn = 1;
    int game_mode;
    int max_rounds = 6;
    bool ai_first = false;
    menu(&turn, &game_mode, &max_rounds, &ai_first);
    board game_board = create_board();
    while (!is_winning(game_board) && max_rounds > round) {
        board P = game_board;
        output_board(game_board);
        int place;
        if (game_mode == 1) {
            if (turn == 1) {
                place = player_play(game_board, round, 1);
                turn = -1;
            } else {
                place = player_play(game_board, round, -1);
                turn = 1;
            }
        } else if (game_mode == 2) {
            if (turn == 1) {
                place = player_play(game_board, round, (ai_first ? -1 : 1));
                turn = -1;
            } else {
                place = ai_play(game_board, round, ai_first, max_rounds - 1);
                turn = 1;
            }
        } else {
            if (turn == 1) {
                place = ai_play(game_board, round, false, max_rounds - 1);
                turn = -1;
            } else {
                place = ai_play(game_board, round, true, max_rounds - 1);
                turn = 1;
            }
        }
        game_board = next_board(game_board, place, round);
        if (game_board == nullptr) {
            printf("There was an error, try again\n");
            game_board = P;
        } else {
            free(P);
            round++;
        }
    }
    output_board(game_board);
    if (is_winning(game_board)) {
        printf("Player %c wins !!!\n", ((is_winning(game_board) == 1) ? 'X' : 'O'));
    } else {
        printf("Tie\n");
    }
    free(game_board);
    return 0;
}
