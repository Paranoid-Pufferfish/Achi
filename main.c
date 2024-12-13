#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "game_board.h"
#include "decision_tree.h"
#include "game_interface.h"

#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768
#define BOARD_DIMS (SCREEN_HEIGHT-300)
#define PLAYER_SIZE 75
#define EMPTY_SIZE 25

typedef enum ACHI_SCENE {
    ACHI_MENU,
    ACHI_ABOUT,
    ACHI_PREGAME_ROUNDS,
    ACHI_PREGAME_PVA,
    ACHI_PREGAME_AVA,
    ACHI_GAME_START,
    ACHI_END
} ACHI_SCENE;

typedef enum GAME_MODE {
    NONE,
    GAME_MODE_PVP,
    GAME_MODE_PVA,
    GAME_MODE_AVA
} GAME_MODE;

const int adjacencyMatrix2[9][3] = {
    {1, 3, 4},
    {0, 2, 4},
    {1, 5, 4},
    {0, 6, 4},
    {4, 4, 4}, // Place holder
    {2, 8, 4},
    {3, 7, 4},
    {6, 8, 4},
    {7, 5, 4}
};
const int adjacent_to_center[8] = {0, 1, 2, 3, 5, 6, 7, 8};

int main(void) {
    SDL_Window *window;
    SDL_Renderer *renderer;
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD) || !TTF_Init()) {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Error initializing SDL : %s\n", SDL_GetError());
        return 1;
    }
    if (!SDL_CreateWindowAndRenderer("Achi Game", SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window,
                                     &renderer)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_RENDER, "Error Creating Window and Renderer : %s\n", SDL_GetError());
        return 1;
    }
    ACHI_SCENE scene = ACHI_MENU;
    bool quit = false;
    int max_rounds = 0;
    int round = 1;
    int selected = -1;
    GAME_MODE game_mode = NONE;
    bool ai_first = true;
    bool order_selected = false;
    TTF_Font *font = TTF_OpenFont("../media/Acme 9 Regular.ttf", 30);
    TTF_Font *font_underlined = TTF_OpenFont("../media/Acme 9 Regular.ttf", 30);
    TTF_SetFontStyle(font_underlined,TTF_STYLE_UNDERLINE);
    TTF_TextEngine *text_engine = TTF_CreateRendererTextEngine(renderer);
    SDL_Cursor *pointing_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
    SDL_Cursor *default_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
    char buf[1024] = {0};
    SDL_Event event;
    SDL_FRect graphical_board = {
        (float) (SCREEN_WIDTH - BOARD_DIMS) / 2, (float) (SCREEN_HEIGHT - BOARD_DIMS) / 2 + 70, BOARD_DIMS, BOARD_DIMS
    };
    board game_board = nullptr;
    board board_cleaner = nullptr;
    SDL_FPoint hot_points[9];
    hot_points[0] = (SDL_FPoint){graphical_board.x, graphical_board.y};
    hot_points[1] = (SDL_FPoint){graphical_board.x + (float) BOARD_DIMS / 2, graphical_board.y};
    hot_points[2] = (SDL_FPoint){graphical_board.x + (float) BOARD_DIMS, graphical_board.y};
    hot_points[3] = (SDL_FPoint){graphical_board.x, graphical_board.y + (float) BOARD_DIMS / 2};
    hot_points[4] = (SDL_FPoint){
        graphical_board.x + (float) BOARD_DIMS / 2, graphical_board.y + (float) BOARD_DIMS / 2
    };
    hot_points[5] = (SDL_FPoint){graphical_board.x + (float) BOARD_DIMS, graphical_board.y + (float) BOARD_DIMS / 2};
    hot_points[6] = (SDL_FPoint){graphical_board.x, graphical_board.y + (float) BOARD_DIMS};
    hot_points[7] = (SDL_FPoint){graphical_board.x + (float) BOARD_DIMS / 2, graphical_board.y + (float) BOARD_DIMS};
    hot_points[8] = (SDL_FPoint){graphical_board.x + (float) BOARD_DIMS, graphical_board.y + (float) BOARD_DIMS};
    SDL_Texture *unoccupied_square = IMG_LoadTexture(renderer, "../media/unoccupied_piece.svg");
    SDL_FRect squares[9];
    if (unoccupied_square == nullptr) {
        SDL_Log("Cannot import assets : %s\n", SDL_GetError());
        return 1;
    }
    SDL_Texture *player_occupied = IMG_LoadTexture(renderer, "../media/player_piece.svg");
    if (player_occupied == nullptr) {
        SDL_Log("Cannot import assets : %s\n", SDL_GetError());
        return 1;
    }
    TTF_Text *WELCOME_text = TTF_CreateText(text_engine, font, "Welcome to the Achi game!", 0);
    TTF_Text *PVP_text = TTF_CreateText(text_engine, font, "1) Player VS Player", 0);
    TTF_Text *PVA_text = TTF_CreateText(text_engine, font, "2) Player VS AI", 0);
    TTF_Text *AVA_text = TTF_CreateText(text_engine, font, "3) AI VS AI", 0);
    TTF_Text *ABOUT_text = TTF_CreateText(text_engine, font, "4) About", 0);
    TTF_Text *QUIT_text = TTF_CreateText(text_engine, font, "5) Quit to desktop", 0);
    TTF_SetTextColorFloat(QUIT_text, 0xFF, 0x00, 0x00,SDL_ALPHA_OPAQUE_FLOAT);
    TTF_Text *TITLE_text = nullptr;
    TTF_Text *INPUT_text = nullptr;
    TTF_Text *ROUND_text = nullptr;
    TTF_Text *ROUNDS_text = TTF_CreateText(text_engine, font, "How many max_rounds do you want to play? (Minimum 6)",
                                           0);
    TTF_Text *NEXT_text = TTF_CreateText(text_engine, font_underlined, "Next", 0);
    TTF_Text *BACK_text = TTF_CreateText(text_engine, font_underlined, "Go Back", 0);
    TTF_Text *PVP_TITLE_text = TTF_CreateText(text_engine, font, "Player VS Player Mode", 0);
    TTF_Text *PVA_TITLE_text = TTF_CreateText(text_engine, font, "Player VS Minimax Mode", 0);
    TTF_Text *AVA_TITLE_text = TTF_CreateText(text_engine, font, "Minimax VS Minimax Mode", 0);
    TTF_Text *ORDER_text = TTF_CreateText(text_engine, font, "Who would you like to start first ?", 0);
    TTF_Text *AIFirst_text = TTF_CreateText(text_engine, font, "Minimax", 0);
    TTF_Text *PlayerFirst_text = TTF_CreateText(text_engine, font, "Player", 0);
    int text_w = 0;
    int text_h = 0;
    TTF_GetTextSize(WELCOME_text, &text_w, &text_h);
    float WELCOME_x = (float) (SCREEN_WIDTH - text_w) / 2;

    TTF_GetTextSize(ORDER_text, &text_w, &text_h);
    float ORDER_x = (float) (SCREEN_WIDTH - text_w) / 2;

    TTF_GetTextSize(ROUNDS_text, &text_w, &text_h);
    float ROUNDS_x = (float) (SCREEN_WIDTH - text_w) / 2;

    TTF_GetTextSize(BACK_text, &text_w, &text_h);
    SDL_FRect BACK_rect = {0, (float) (SCREEN_HEIGHT - text_h), (float) text_w, (float) text_h};

    TTF_GetTextSize(PVP_text, &text_w, &text_h);
    SDL_FRect PVP_rect = {0, 200, (float) text_w, (float) text_h};

    TTF_GetTextSize(PVA_text, &text_w, &text_h);
    SDL_FRect PVA_rect = {0, 300, (float) text_w, (float) text_h};

    TTF_GetTextSize(AVA_text, &text_w, &text_h);
    SDL_FRect AVA_rect = {0, 400, (float) text_w, (float) text_h};

    TTF_GetTextSize(ABOUT_text, &text_w, &text_h);
    SDL_FRect ABOUT_rect = {0, 500, (float) text_w, (float) text_h};

    TTF_GetTextSize(QUIT_text, &text_w, &text_h);
    SDL_FRect QUIT_rect = {0, 600, (float) text_w, (float) text_h};

    TTF_GetTextSize(AIFirst_text, &text_w, &text_h);
    SDL_FRect AIFirst_rect = {(float) (SCREEN_WIDTH - text_w) / 4, 400, (float) text_w, (float) text_h};

    TTF_GetTextSize(PlayerFirst_text, &text_w, &text_h);
    SDL_FRect PlayerFirst_rect = {3 * (float) (SCREEN_WIDTH - text_w) / 4, 400, (float) text_w, (float) text_h};

    TTF_GetTextSize(NEXT_text, &text_w, &text_h);
    TTF_DrawRendererText(NEXT_text, SCREEN_WIDTH - text_w, SCREEN_HEIGHT - text_h);
    SDL_FRect NEXT_rect = {SCREEN_WIDTH - text_w, SCREEN_HEIGHT - text_h, (float) text_w, (float) text_h};
    SDL_FPoint mouse;
    SDL_FRect DEBUG_rect = {0, 0, 100, 100};
    SDL_PropertiesID input_properties_id = SDL_CreateProperties();
    SDL_SetNumberProperty(input_properties_id,SDL_PROP_TEXTINPUT_TYPE_NUMBER, SDL_TEXTINPUT_TYPE_NUMBER);
    while (!quit) {
        float x_pos = 0;
        float y_pos = 0;
        SDL_GetMouseState(&x_pos, &y_pos);
        mouse.x = x_pos;
        mouse.y = y_pos;
        SDL_RenderClear(renderer);
        SDL_SetCursor(default_cursor);
        switch (scene) {
            case ACHI_MENU:
                TTF_DrawRendererText(WELCOME_text, WELCOME_x, 0);
                TTF_DrawRendererText(PVP_text, 0, 200);
                TTF_DrawRendererText(PVA_text, 0, 300);
                TTF_DrawRendererText(AVA_text, 0, 400);
                TTF_DrawRendererText(ABOUT_text, 0, 500);
                TTF_DrawRendererText(QUIT_text, 0, 600);
                if (SDL_PointInRectFloat(&mouse, &PVP_rect) ||
                    SDL_PointInRectFloat(&mouse, &PVA_rect) || SDL_PointInRectFloat(&mouse, &AVA_rect) ||
                    SDL_PointInRectFloat(&mouse, &ABOUT_rect) || SDL_PointInRectFloat(&mouse, &QUIT_rect))
                    SDL_SetCursor(pointing_cursor);
                if (SDL_PollEvent(&event)) {
                    switch (event.type) {
                        case SDL_EVENT_QUIT: quit = true;
                            break;
                        case SDL_EVENT_GAMEPAD_BUTTON_UP:
                            SDL_Log("BUTTON A PRESSED");
                        case SDL_EVENT_MOUSE_BUTTON_UP:
                            if (event.button.button == SDL_BUTTON_LEFT) {
                                if (SDL_PointInRectFloat(&mouse, &PVP_rect)) {
                                    scene = ACHI_PREGAME_ROUNDS;
                                    TITLE_text = PVP_TITLE_text;
                                    game_mode = GAME_MODE_PVP;
                                }
                                if (SDL_PointInRectFloat(&mouse, &PVA_rect)) {
                                    scene = ACHI_PREGAME_ROUNDS;
                                    TITLE_text = PVA_TITLE_text;
                                    game_mode = GAME_MODE_PVA;
                                }
                                if (SDL_PointInRectFloat(&mouse, &AVA_rect)) {
                                    scene = ACHI_PREGAME_ROUNDS;
                                    TITLE_text = AVA_TITLE_text;
                                    game_mode = GAME_MODE_AVA;
                                }
                                if (SDL_PointInRectFloat(&mouse, &ABOUT_rect)) {
                                    scene = ACHI_ABOUT;
                                }
                                if (SDL_PointInRectFloat(&mouse, &QUIT_rect)) {
                                    quit = true;
                                }
                            }
                            break;
                        default:
                            break;
                    }
                }
                break;
            case ACHI_PREGAME_ROUNDS:
                TTF_GetTextSize(TITLE_text, &text_w, &text_h);
                TTF_DrawRendererText(TITLE_text, (float) (SCREEN_WIDTH - text_w) / 2, 0);
                TTF_DrawRendererText(BACK_text, 0, BACK_rect.y);
                TTF_DrawRendererText(ROUNDS_text, ROUNDS_x, 200);
                if (INPUT_text != nullptr)
                    TTF_DestroyText(INPUT_text);
                INPUT_text = TTF_CreateText(text_engine, font, buf, 0);
                TTF_GetTextSize(INPUT_text, &text_w, &text_h);
                TTF_DrawRendererText(INPUT_text, (float) (SCREEN_WIDTH - text_w) / 2, 300);
                if (strlen(buf) > 0)
                    max_rounds = (int) strtol(buf, nullptr, 10);
                else
                    max_rounds = 0;
                SDL_StartTextInputWithProperties(window, input_properties_id);
                if (SDL_PollEvent(&event)) {
                    switch (event.type) {
                        case SDL_EVENT_QUIT: quit = true;
                            break;
                        case SDL_EVENT_TEXT_INPUT:
                            if (event.text.text[0] >= '0' && event.text.text[0] <= '9' && strlen(buf) < 2)
                                strcat(buf, event.text.text);
                            break;
                        case SDL_EVENT_KEY_DOWN:
                            if (event.key.key == SDLK_BACKSPACE && strlen(buf) > 0)
                                buf[strlen(buf) - 1] = '\0';
                            else if (event.key.key == SDLK_RETURN && max_rounds >= 6) {
                                switch (game_mode) {
                                    case GAME_MODE_PVP: scene = ACHI_GAME_START;
                                        break;
                                    case GAME_MODE_PVA: scene = ACHI_PREGAME_PVA;
                                        break;
                                    case GAME_MODE_AVA: scene = ACHI_PREGAME_AVA;
                                        break;
                                    default: SDL_Log("Shouldn't happen yet here we are");
                                        exit(EXIT_FAILURE);
                                }
                            }
                            break;
                        case SDL_EVENT_MOUSE_BUTTON_UP:
                            if (event.button.button == SDL_BUTTON_LEFT) {
                                if (SDL_PointInRectFloat(&mouse, &NEXT_rect) && max_rounds >= 6) {
                                    switch (game_mode) {
                                        case GAME_MODE_PVP: scene = ACHI_GAME_START;
                                            break;
                                        case GAME_MODE_PVA: scene = ACHI_PREGAME_PVA;
                                            break;
                                        case GAME_MODE_AVA: scene = ACHI_PREGAME_AVA;
                                            break;
                                        default: SDL_Log("Shouldn't happen yet here we are");
                                            exit(EXIT_FAILURE);
                                    }
                                }
                                if (SDL_PointInRectFloat(&mouse, &BACK_rect)) {
                                    scene = ACHI_MENU;
                                    game_mode = NONE;
                                }
                            }
                            break;
                        default: ;
                    }
                }
                SDL_StopTextInput(window);
                if (max_rounds >= 6) {
                    TTF_DrawRendererText(NEXT_text, NEXT_rect.x, NEXT_rect.y);
                    if (SDL_PointInRectFloat(&mouse, &NEXT_rect))
                        SDL_SetCursor(pointing_cursor);
                }
                if (SDL_PointInRectFloat(&mouse, &BACK_rect))
                    SDL_SetCursor(pointing_cursor);

                break;
            case ACHI_PREGAME_PVA:
                TTF_GetTextSize(TITLE_text, &text_w, &text_h);
                TTF_DrawRendererText(TITLE_text, (float) (SCREEN_WIDTH - text_w) / 2, 0);
                TTF_DrawRendererText(BACK_text, 0, BACK_rect.y);
                TTF_DrawRendererText(ORDER_text, ORDER_x, 200);
                TTF_DrawRendererText(AIFirst_text, AIFirst_rect.x, AIFirst_rect.y);
                TTF_DrawRendererText(PlayerFirst_text, PlayerFirst_rect.x, PlayerFirst_rect.y);
                if (SDL_PollEvent(&event)) {
                    switch (event.type) {
                        case SDL_EVENT_QUIT: quit = true;
                            break;
                        case SDL_EVENT_MOUSE_BUTTON_UP:
                            if (SDL_PointInRectFloat(&mouse, &BACK_rect)) {
                                scene = ACHI_PREGAME_ROUNDS;
                            }
                            if (SDL_PointInRectFloat(&mouse, &AIFirst_rect)) {
                                ai_first = true;
                                order_selected = true;
                                TTF_SetTextColorFloat(AIFirst_text, 0xFF, 0x00, 0x00,SDL_ALPHA_OPAQUE_FLOAT);
                                TTF_SetTextColorFloat(PlayerFirst_text, 0xFF, 0xFF, 0xFF,SDL_ALPHA_OPAQUE_FLOAT);
                            }
                            if (SDL_PointInRectFloat(&mouse, &PlayerFirst_rect)) {
                                ai_first = false;
                                order_selected = true;
                                TTF_SetTextColorFloat(PlayerFirst_text, 0xFF, 0x00, 0x00,SDL_ALPHA_OPAQUE_FLOAT);
                                TTF_SetTextColorFloat(AIFirst_text, 0xFF, 0xFF, 0xFF,SDL_ALPHA_OPAQUE_FLOAT);
                            }
                            if (SDL_PointInRectFloat(&mouse, &NEXT_rect) && order_selected)
                                scene = ACHI_GAME_START;
                            break;
                        case SDL_EVENT_KEY_DOWN:
                            if (event.key.key == SDLK_RETURN && order_selected)
                                scene = ACHI_GAME_START;
                            break;
                        default: ;
                    }
                }
                if (order_selected) {
                    TTF_DrawRendererText(NEXT_text, NEXT_rect.x, NEXT_rect.y);
                    if (SDL_PointInRectFloat(&mouse, &NEXT_rect))
                        SDL_SetCursor(pointing_cursor);
                }
                if (SDL_PointInRectFloat(&mouse, &BACK_rect) || SDL_PointInRectFloat(&mouse, &PlayerFirst_rect) ||
                    SDL_PointInRectFloat(&mouse, &AIFirst_rect))
                    SDL_SetCursor(pointing_cursor);
                break;
            case ACHI_PREGAME_AVA: SDL_Log("TODO: AVA, GAME MODE : %d. %d Turns", game_mode, max_rounds);
                scene = ACHI_GAME_START;
                break;
            case ACHI_GAME_START:
                int turn = (round % 2 != 0) ? 1 : 2;
                TTF_GetTextSize(TITLE_text, &text_w, &text_h);
                TTF_DrawRendererText(TITLE_text, (float) (SCREEN_WIDTH - text_w) / 2, 0);
                if (ROUND_text != nullptr)
                    TTF_DestroyText(ROUND_text);
                if (game_board == nullptr) {
                    game_board = create_board();
                    board_cleaner = game_board;
                }
                if (is_winning(game_board) || round > max_rounds)
                    scene = ACHI_END;

                switch (game_mode) {
                    case GAME_MODE_PVA:
                        if (ai_first)
                            sprintf(buf, "Round N°%d - %s", round, (turn == 1) ? "AI's turn" : "Players turns");
                        else
                            sprintf(buf, "Round N°%d - %s", round, (turn == 2) ? "AI's turn" : "Players turns");
                        break;
                    case GAME_MODE_PVP:
                        sprintf(buf, "Round N°%d - Player %d turn", round, turn);
                        break;
                    case GAME_MODE_AVA:
                        sprintf(buf, "Round N°%d - %s", round, (round % 2 != 0) ? "Minimizer turn" : "Maximizer turn");
                    default: ;
                }
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF,SDL_ALPHA_OPAQUE_FLOAT);
                SDL_RenderFillRect(renderer, &DEBUG_rect);
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00,SDL_ALPHA_OPAQUE_FLOAT);
                if (SDL_PointInRectFloat(&mouse, &DEBUG_rect))
                    SDL_SetCursor(pointing_cursor);
                ROUND_text = TTF_CreateText(text_engine, font, buf, 0);
                TTF_GetTextSize(ROUND_text, &text_w, &text_h);
                TTF_DrawRendererText(ROUND_text, (float) (SCREEN_WIDTH - text_w) / 2, 75);
                SDL_SetRenderDrawColor(renderer, 0xCE, 0xF1, 0xF2,SDL_ALPHA_OPAQUE_FLOAT);
                SDL_RenderRect(renderer, &graphical_board);
                SDL_RenderLine(renderer, hot_points[1].x, hot_points[1].y, hot_points[7].x, hot_points[7].y);
                SDL_RenderLine(renderer, hot_points[2].x, hot_points[2].y, hot_points[6].x, hot_points[6].y);
                SDL_RenderLine(renderer, hot_points[3].x, hot_points[3].y, hot_points[5].x, hot_points[5].y);
                SDL_RenderLine(renderer, hot_points[0].x, hot_points[0].y, hot_points[8].x, hot_points[8].y);
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00,SDL_ALPHA_OPAQUE_FLOAT);
                for (int i = 0; i < 9; ++i) {
                    switch (game_board[i].occupied_by) {
                        case -1:
                            squares[i].x = hot_points[i].x - (float) PLAYER_SIZE / 2;
                            squares[i].y = hot_points[i].y - (float) PLAYER_SIZE / 2;
                            squares[i].w = squares[i].h = PLAYER_SIZE;
                            if (i == selected) {
                                squares[i].w = squares[i].h = PLAYER_SIZE + 20;
                                squares[i].x = hot_points[i].x - (float) (PLAYER_SIZE + 20) / 2;
                                squares[i].y = hot_points[i].y - (float) (PLAYER_SIZE + 20) / 2;
                            } else
                                squares[i].w = squares[i].h = PLAYER_SIZE;
                            SDL_SetTextureColorModFloat(player_occupied, 0xcf, 0xff, 0xdd);
                            SDL_RenderTexture(renderer, player_occupied, nullptr, &squares[i]);
                            break;
                        case 0:
                            squares[i].x = hot_points[i].x - (float) EMPTY_SIZE / 2;
                            squares[i].y = hot_points[i].y - (float) EMPTY_SIZE / 2;
                            squares[i].w = squares[i].h = EMPTY_SIZE;
                            if (round > 6 && selected != -1) {
                                if (selected == 4 || i == 4)
                                    SDL_SetTextureColorModFloat(unoccupied_square, 0xFF, 0x00, 0x00);
                                else {
                                    for (int j = 0; j < 2; ++j) {
                                        if (i == adjacencyMatrix2[selected][j]) {
                                            SDL_SetTextureColorModFloat(unoccupied_square, 0xFF, 0x00, 0x00);
                                            break;
                                        } else
                                            SDL_SetTextureColorModFloat(unoccupied_square, 0xFF, 0xff, 0xFF);
                                    }
                                }
                            } else
                                SDL_SetTextureColorModFloat(unoccupied_square, 0xFF, 0xff, 0xFF);
                            SDL_RenderTexture(renderer, unoccupied_square, nullptr, &squares[i]);
                            break;
                        case 1:
                            squares[i].x = hot_points[i].x - (float) PLAYER_SIZE / 2;
                            squares[i].y = hot_points[i].y - (float) PLAYER_SIZE / 2;
                            squares[i].w = squares[i].h = PLAYER_SIZE;
                            if (i == selected) {
                                squares[i].w = squares[i].h = PLAYER_SIZE + 20;
                                squares[i].x = hot_points[i].x - (float) (PLAYER_SIZE + 20) / 2;
                                squares[i].y = hot_points[i].y - (float) (PLAYER_SIZE + 20) / 2;
                            } else
                                squares[i].w = squares[i].h = PLAYER_SIZE;
                            SDL_SetTextureColorModFloat(player_occupied, 0xd0, 0xba, 0xff);
                            SDL_RenderTexture(renderer, player_occupied, nullptr, &squares[i]);
                            break;
                        default: ;
                    }
                }
                if (SDL_PollEvent(&event)) {
                    switch (event.type) {
                        case SDL_EVENT_QUIT: quit = true;
                            break;
                        case SDL_EVENT_MOUSE_BUTTON_UP:
                            if (SDL_PointInRectFloat(&mouse, &DEBUG_rect)) {
                                if (event.button.button == SDL_BUTTON_LEFT) {
                                    round++;
                                    selected = -1;
                                } else {
                                    round--;
                                    selected = -1;
                                }
                            }
                            if (game_mode == GAME_MODE_PVP) {
                                if (round <= 6) {
                                    for (int i = 0; i < 9; ++i) {
                                        if (game_board[i].occupied_by == 0 &&
                                            SDL_PointInRectFloat(&mouse, &squares[i])) {
                                            game_board = next_board(game_board, i, round++);
                                            free(board_cleaner);
                                            board_cleaner = game_board;
                                        }
                                    }
                                } else {
                                    for (int i = 0; i < 9; ++i) {
                                        if ((game_board[i].occupied_by == ((turn == 2) ? -1 : turn)) &&
                                            SDL_PointInRectFloat(&mouse, &squares[i])) {
                                            selected = i;
                                        }
                                        if (selected != -1) {
                                            int number_played = 0;
                                            int player_squares[3] = {-1, -1, -1};
                                            int number_adjacent = 0;
                                            int adjacent_squares[3] = {-1, -1, -1};
                                            int selected_index;
                                            get_played(game_board, &number_played, ((turn == 2) ? -1 : turn),
                                                       player_squares);
                                            for (int j = 0; j < number_played; ++j) {
                                                if (selected == player_squares[j]) {
                                                    selected_index=j;
                                                    break;
                                                }
                                            }
                                            get_adjacent(game_board, &number_adjacent, selected, adjacent_squares);
                                            for (int j = 0; j < number_adjacent; ++j) {
                                                int current_adjacent = (selected == 4)
                                                                           ? adjacent_to_center[adjacent_squares[j]]
                                                                           : adjacencyMatrix2[selected][adjacent_squares
                                                                               [j]];
                                                if (SDL_PointInRectFloat(&mouse, &squares[current_adjacent])) {
                                                    selected = -1;
                                                    game_board = next_board(game_board,selected_index*3+j,round++);
                                                    free(board_cleaner);
                                                    board_cleaner = game_board;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            break;
                        default: ;
                    }
                }
                if (round <= 6) {
                    for (int i = 0; i < 9; ++i) {
                        if (game_board[i].occupied_by == 0 && SDL_PointInRectFloat(&mouse, &squares[i]))
                            SDL_SetCursor(pointing_cursor);
                    }
                } else {
                    for (int i = 0; i < 9; ++i) {
                        if ((game_board[i].occupied_by == ((turn == 2) ? -1 : turn)) && SDL_PointInRectFloat(
                                &mouse, &squares[i]))
                            SDL_SetCursor(pointing_cursor);
                    }
                }
            if (selected != -1 && game_board[selected].occupied_by != ((turn == 2) ? -1 : turn)){
                selected = -1;
            }
                break;
            case ACHI_END: SDL_Log("TODO: END");
                quit = true;
                break;
            case ACHI_ABOUT: SDL_Log("TODO: ABOUT");
                quit = true;
                break;
        }
        SDL_RenderPresent(renderer);
    }
    TTF_DestroyRendererTextEngine(text_engine);
    TTF_CloseFont(font);
    TTF_CloseFont(font_underlined);
    TTF_Quit();
    SDL_DestroyCursor(default_cursor);
    SDL_DestroyCursor(pointing_cursor);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
