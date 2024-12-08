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

typedef enum ACHI_SCENE {
    ACHI_MENU,
    ACHI_ABOUT,
    ACHI_PREGAME_PVA,
    ACHI_PREGAME_PVP,
    ACHI_PREGAME_AVA,
    ACHI_GAME_START,
    ACHI_END
} ACHI_SCENE;

void achi_menu(SDL_Renderer *renderer, TTF_Font *font, TTF_TextEngine *text_engine, SDL_Cursor *pointing_cursor,
               bool *quit, ACHI_SCENE *scene) {
    SDL_Event event;
    TTF_Text *WELCOME_text = TTF_CreateText(text_engine, font, "Welcome to the Achi game!", 0);
    TTF_Text *PVP_text = TTF_CreateText(text_engine, font, "1) Player VS Player", 0);
    TTF_Text *PVA_text = TTF_CreateText(text_engine, font, "2) Player VS AI", 0);
    TTF_Text *AVA_text = TTF_CreateText(text_engine, font, "3) AI VS AI", 0);
    TTF_Text *ABOUT_text = TTF_CreateText(text_engine, font, "4) About", 0);
    TTF_Text *QUIT_text = TTF_CreateText(text_engine, font, "5) Quit to desktop", 0);
    TTF_SetTextColorFloat(QUIT_text, 0xFF, 0x00, 0x00,SDL_ALPHA_OPAQUE_FLOAT);
    float x_pos = 0;
    float y_pos = 0;
    SDL_GetMouseState(&x_pos, &y_pos);
    SDL_FPoint mouse = {x_pos, y_pos};
    int text_w = 0;
    int text_h = 0;
    TTF_GetTextSize(WELCOME_text, &text_w, &text_h);
    TTF_DrawRendererText(WELCOME_text, (float) (SCREEN_WIDTH - text_w) / 2, 0);

    TTF_GetTextSize(PVP_text, &text_w, &text_h);
    TTF_DrawRendererText(PVP_text, 0, 200);
    SDL_FRect PVP_rect = {0, 200, (float) text_w, (float) text_h};

    TTF_GetTextSize(PVA_text, &text_w, &text_h);
    TTF_DrawRendererText(PVA_text, 0, 300);
    SDL_FRect PVA_rect = {0, 300, (float) text_w, (float) text_h};


    TTF_GetTextSize(AVA_text, &text_w, &text_h);
    TTF_DrawRendererText(AVA_text, 0, 400);
    SDL_FRect AVA_rect = {0, 400, (float) text_w, (float) text_h};

    TTF_GetTextSize(ABOUT_text, &text_w, &text_h);
    TTF_DrawRendererText(ABOUT_text, 0, 500);
    SDL_FRect ABOUT_rect = {0, 500, (float) text_w, (float) text_h};

    TTF_GetTextSize(QUIT_text, &text_w, &text_h);
    TTF_DrawRendererText(QUIT_text, 0, 600);
    SDL_FRect QUIT_rect = {0, 600, (float) text_w, (float) text_h};
    if (SDL_PointInRectFloat(&mouse, &PVP_rect) ||
        SDL_PointInRectFloat(&mouse, &PVA_rect) || SDL_PointInRectFloat(&mouse, &AVA_rect) ||
        SDL_PointInRectFloat(&mouse, &ABOUT_rect) || SDL_PointInRectFloat(&mouse, &QUIT_rect))
        SDL_SetCursor(pointing_cursor);
    if (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT: *quit = true;
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (SDL_PointInRectFloat(&mouse, &PVP_rect))
                        *scene = ACHI_PREGAME_PVP;
                    if (SDL_PointInRectFloat(&mouse, &PVA_rect))
                        *scene = ACHI_PREGAME_PVA;
                    if (SDL_PointInRectFloat(&mouse, &AVA_rect))
                        *scene = ACHI_PREGAME_AVA;
                    if (SDL_PointInRectFloat(&mouse, &ABOUT_rect))
                        *scene = ACHI_ABOUT;
                    if (SDL_PointInRectFloat(&mouse, &QUIT_rect))
                        *quit = true;
                }
                break;
            default:
                break;
        }
    }
    SDL_RenderPresent(renderer);
    TTF_DestroyText(WELCOME_text);
    TTF_DestroyText(PVA_text);
    TTF_DestroyText(PVP_text);
    TTF_DestroyText(AVA_text);
    TTF_DestroyText(QUIT_text);
}

void achi_pregame_pvp(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, TTF_Font *font_underline,
                      TTF_TextEngine *text_engine, SDL_Cursor *pointing_cursor, int *turns
                      , int *game_mode, bool *quit, ACHI_SCENE *scene, char *buf) {
    float x_pos = 0;
    float y_pos = 0;
    SDL_GetMouseState(&x_pos, &y_pos);
    SDL_FPoint mouse = {x_pos, y_pos};
    SDL_FRect NEXT_rect;
    SDL_Event event;
    TTF_Text *TITLE_text = TTF_CreateText(text_engine, font, "Player VS Player Mode", 0);
    TTF_Text *ROUNDS_text = TTF_CreateText(text_engine, font, "How many rounds do you want to play?", 0);
    TTF_Text *NEXT_text = TTF_CreateText(text_engine, font_underline, "Next", 0);
    *turns = (int) strtol(buf, nullptr, 10);
    TTF_Text *INPUT_text = TTF_CreateText(text_engine, font, buf, 0);
    int text_w = 0;
    int text_h = 0;
    TTF_GetTextSize(TITLE_text, &text_w, &text_h);
    TTF_DrawRendererText(TITLE_text, (float) (SCREEN_WIDTH - text_w) / 2, 0);
    TTF_GetTextSize(ROUNDS_text, &text_w, &text_h);
    TTF_DrawRendererText(ROUNDS_text, (float) (SCREEN_WIDTH - text_w) / 2, 200);
    TTF_GetTextSize(INPUT_text, &text_w, &text_h);
    TTF_DrawRendererText(INPUT_text, (float) (SCREEN_WIDTH - text_w) / 2, 300);
    if (*turns > 0) {
        TTF_GetTextSize(NEXT_text, &text_w, &text_h);
        TTF_DrawRendererText(NEXT_text, SCREEN_WIDTH - text_w, SCREEN_HEIGHT - text_h);
        const SDL_FRect temp_rect = {SCREEN_WIDTH - text_w, SCREEN_HEIGHT - text_h, (float) text_w, (float) text_h};
        NEXT_rect = temp_rect;
        if (SDL_PointInRectFloat(&mouse, &NEXT_rect))
            SDL_SetCursor(pointing_cursor);
    }
    SDL_PropertiesID input_properties_id = SDL_CreateProperties();
    SDL_SetNumberProperty(input_properties_id,SDL_PROP_TEXTINPUT_TYPE_NUMBER, SDL_TEXTINPUT_TYPE_NUMBER);
    SDL_StartTextInputWithProperties(window, input_properties_id);
    if (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT: *quit = true;
                break;
            case SDL_EVENT_TEXT_INPUT:
                if (event.text.text[0] >= '0' && event.text.text[0] <= '9' && strlen(buf) < 2)
                    strcat(buf, event.text.text);
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_BACKSPACE && strlen(buf) > 0)
                    buf[strlen(buf) - 1] = '\0';
                else if (event.key.key == SDLK_RETURN && *turns > 0)
                    *scene = ACHI_GAME_START;
                *game_mode = 1;
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (SDL_PointInRectFloat(&mouse, &NEXT_rect) && *turns > 0)
                        *scene = ACHI_GAME_START;
                    *game_mode = 1;
                }
                break;
            default:
                break;
        }
    }
    SDL_StopTextInput(window);
    SDL_RenderPresent(renderer);
    TTF_DestroyText(TITLE_text);
    TTF_DestroyText(ROUNDS_text);
    TTF_DestroyText(INPUT_text);
    TTF_DestroyText(NEXT_text);
}

int main(void) {
    SDL_Window *window;
    SDL_Renderer *renderer;
    if (!SDL_Init(!SDL_INIT_VIDEO) || !TTF_Init()) {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Error initializing SDL : %s\n", SDL_GetError());
        return 1;
    }
    if (!SDL_CreateWindowAndRenderer("Achi Game", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &window,
                                     &renderer)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_RENDER, "Error Creating Window and Renderer : %s\n", SDL_GetError());
        return 1;
    }

    ACHI_SCENE scene = ACHI_MENU;
    bool quit = false;
    int turns = 0;
    int game_mode = 0;
    TTF_Font *font = TTF_OpenFont("../media/Acme 9 Regular.ttf", 30);
    TTF_Font *font_underlined = TTF_OpenFont("../media/Acme 9 Regular.ttf", 30);
    TTF_SetFontStyle(font_underlined,TTF_STYLE_UNDERLINE);
    TTF_TextEngine *text_engine = TTF_CreateRendererTextEngine(renderer);
    SDL_Cursor *pointing = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
    SDL_Cursor *default_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
    char buf[1024] = {0};
    while (!quit) {
        SDL_RenderClear(renderer);
        SDL_SetCursor(default_cursor);
        switch (scene) {
            case ACHI_MENU: achi_menu(renderer, font, text_engine, pointing, &quit, &scene);
                break;
            case ACHI_PREGAME_PVP: achi_pregame_pvp(window, renderer, font, font_underlined, text_engine, pointing,
                                                    &turns
                                                    , &game_mode, &quit, &scene,
                                                    buf);
                break;
            case ACHI_PREGAME_PVA: SDL_Log("TODO: PVA");
                quit = true;
                break;
            case ACHI_PREGAME_AVA: SDL_Log("TODO: AVA");
                quit = true;
                break;
            case ACHI_GAME_START: SDL_Log("TODO: GAME START");
                quit = true;
                break;
            case ACHI_END: SDL_Log("TODO: END");
                quit = true;
                break;
            case ACHI_ABOUT: SDL_Log("TODO: ABOUT");
                quit = true;
                break;
        }
    }
    /*int round = 1;
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
                place = ai_play(game_board, round, ai_first, max_rounds-1);
                turn = 1;
            }
        } else {
            if (turn == 1) {
                place = ai_play(game_board, round, false, max_rounds-1);
                turn = -1;
            } else {
                place = ai_play(game_board, round, true, max_rounds-1);
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
    free(game_board);*/
    TTF_DestroyRendererTextEngine(text_engine);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyCursor(default_cursor);
    SDL_DestroyCursor(pointing);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
