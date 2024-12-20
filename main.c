#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#ifdef SDL_PLATFORM_WINDOWS
#include <time.h>
#endif
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "game_board.h"
#include "decision_tree.h"
#include "game_interface.h"

#define DLS_LIMIT 8
#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768
#define BOARD_DIMS (SCREEN_HEIGHT-300)
#define PLAYER_SIZE 75
#define EMPTY_SIZE 25
#define CREDITS "Authors:\n- MOUHOUS Mathya (G3)\n- AIT MEDDOUR Fouâd-Eddine (G1)\nSoftware Used:\n- SDL3 master (https://github.com/libsdl-org/SDL)\n- SDL3_ttf master (https://github.com/libsdl-org/SDL_ttf)\n- SDL3_image master (https://github.com/libsdl-org/SDL_image)\n- CMake 3.30.6 (https://gitlab.kitware.com/cmake/cmake)\nFont : Acme 9 Regular\nTested On :\n- Ubuntu 24.10\n- Gentoo amd64 Stable\n- Windows 10 KVM/QEMU\n- Windows 11"

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

typedef enum RANDOMNESS {
    NO_RAND,
    SOME_RAND,
    ALL_RAND
} RANDOMNESS;

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
#ifdef SDL_PLATFORM_WINDOWS
    srand(time(0));
#endif
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
    bool skip_cycle = false;
    int max_rounds = 0;
    int round = 1;
    int selected = -1;
    int state = 0;
    GAME_MODE game_mode = NONE;
    RANDOMNESS randomness_first_ai = NO_RAND;
    RANDOMNESS randomness_second_ai = NO_RAND;
    bool ai_first = true;
    bool order_selected = false;
    bool second_ai_rand_selected = false;
    SDL_SetWindowIcon(window, IMG_Load("../media/miku.png"));
    TTF_Font *font = TTF_OpenFont("../media/Acme 9 Regular.ttf", 30);
    TTF_Font *font_underlined = TTF_OpenFont("../media/Acme 9 Regular.ttf", 30);
    TTF_Font *font_credits = TTF_OpenFont("../media/Acme 9 Regular.ttf", 23);
    TTF_SetFontWrapAlignment(font_credits, TTF_HORIZONTAL_ALIGN_CENTER);
    TTF_SetFontStyle(font_underlined,TTF_STYLE_UNDERLINE);
    TTF_TextEngine *text_engine = TTF_CreateRendererTextEngine(renderer);
    SDL_Cursor *pointing_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
    SDL_Cursor *default_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
    char buf[1024] = {0};
    char ai_desc[100] = {0};
    char ai2_desc[100] = {0};
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
    TTF_Text *ROUNDS_text = TTF_CreateText(text_engine, font, "How many rounds do you want to play? (Minimum 6)",
                                           0);
    TTF_Text *NEXT_text = TTF_CreateText(text_engine, font_underlined, "Next", 0);
    TTF_Text *BACK_text = TTF_CreateText(text_engine, font_underlined, "Go Back", 0);
    TTF_Text *PVP_TITLE_text = TTF_CreateText(text_engine, font, "Player VS Player Mode", 0);
    TTF_Text *PVA_TITLE_text = TTF_CreateText(text_engine, font, "Player VS Minimax Mode", 0);
    TTF_Text *AVA_TITLE_text = TTF_CreateText(text_engine, font, "Minimax VS Minimax Mode", 0);
    TTF_Text *ORDER_text = TTF_CreateText(text_engine, font, "Who would you like to start first ?", 0);
    TTF_Text *ENTROPY_text = TTF_CreateText(text_engine, font, "Would you like to add Randomness ?", 0);
    TTF_Text *AIFirst_text = TTF_CreateText(text_engine, font, "Minimax", 0);
    TTF_Text *AI_IS_THINKING_text = TTF_CreateText(text_engine, font, "AI is thinking...", 0);
    TTF_Text *PlayerFirst_text = TTF_CreateText(text_engine, font, "Player", 0);
    TTF_Text *AI_ONE_text = TTF_CreateText(text_engine, font_underlined, "AI N°1 Mode: ", 0);
    TTF_Text *AI_TWO_text = TTF_CreateText(text_engine, font_underlined, "AI N°2 Mode: ", 0);
    TTF_Text *NO_RANDOMNESS_text = TTF_CreateText(text_engine, font, "No Randomness", 0);
    TTF_Text *SOME_RANDOMNESS_text = TTF_CreateText(text_engine, font, "Some Randomness", 0);
    TTF_Text *ALL_RANDOMNESS_text = TTF_CreateText(text_engine, font, "All Randomness", 0);
    TTF_Text *NO_RANDOMNESS2_text = TTF_CreateText(text_engine, font, "No Randomness", 0);
    TTF_Text *SOME_RANDOMNESS2_text = TTF_CreateText(text_engine, font, "Some Randomness", 0);
    TTF_Text *ALL_RANDOMNESS2_text = TTF_CreateText(text_engine, font, "All Randomness", 0);
    TTF_Text *RETRY_text = TTF_CreateText(text_engine, font_underlined, "Play Again", 0);
    TTF_Text *MAIN_MENU_text = TTF_CreateText(text_engine, font_underlined, "Main Menu", 0);
    TTF_Text *ABOUT_TITLE_text = TTF_CreateText(text_engine, font, "About the project", 0);
    SDL_Surface *CREDITS_text_surface = TTF_RenderText_Blended_Wrapped(font_credits, CREDITS, 0,
                                                                       (SDL_Color){0xFF, 0xFF, 0xFF,SDL_ALPHA_OPAQUE},
                                                                       0);
    SDL_Texture *CREDITS_text_texture = SDL_CreateTextureFromSurface(renderer, CREDITS_text_surface);
    float CREDITS_w;
    float CREDITS_h;
    SDL_GetTextureSize(CREDITS_text_texture, &CREDITS_w, &CREDITS_h);
    SDL_FRect CREDITS_rect = {(SCREEN_WIDTH - CREDITS_w) / 2, 75, CREDITS_w, CREDITS_h};
    SDL_DestroySurface(CREDITS_text_surface);
    int text_w = 0;
    int text_h = 0;
    TTF_GetTextSize(WELCOME_text, &text_w, &text_h);
    float WELCOME_x = (float) (SCREEN_WIDTH - text_w) / 2;

    TTF_GetTextSize(ORDER_text, &text_w, &text_h);
    float ORDER_x = (float) (SCREEN_WIDTH - text_w) / 2;
    TTF_GetTextSize(ABOUT_TITLE_text, &text_w, &text_h);
    float ABOUT_TITLE_x = (float) (SCREEN_WIDTH - text_w) / 2;
    TTF_GetTextSize(ROUNDS_text, &text_w, &text_h);
    float ROUNDS_x = (float) (SCREEN_WIDTH - text_w) / 2;
    TTF_GetTextSize(AI_IS_THINKING_text, &text_w, &text_h);
    float AI_THINKING_x = (float) (SCREEN_WIDTH - text_w) / 2;
    float AI_THINKING_y = (float) (SCREEN_HEIGHT - text_h);

    TTF_GetTextSize(BACK_text, &text_w, &text_h);
    SDL_FRect BACK_rect = {0, (float) (SCREEN_HEIGHT - text_h), (float) text_w, (float) text_h};

    TTF_GetTextSize(RETRY_text, &text_w, &text_h);
    SDL_FRect RETRY_rect = {0, (float) (SCREEN_HEIGHT - text_h) / 2, (float) text_w, (float) text_h};

    TTF_GetTextSize(MAIN_MENU_text, &text_w, &text_h);
    SDL_FRect MAIN_MENU_rect = {
        (float) (SCREEN_WIDTH - text_w), (float) (SCREEN_HEIGHT - text_h) / 2, (float) text_w, (float) text_h
    };

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

    TTF_GetTextSize(ENTROPY_text, &text_w, &text_h);
    SDL_FRect ENTROPY_rect = {(float) (SCREEN_WIDTH - text_w) / 2, 100, (float) text_w, (float) text_h};

    TTF_GetTextSize(AI_ONE_text, &text_w, &text_h);
    SDL_FRect AI_ONE_rect = {(float) (SCREEN_WIDTH - text_w) / 2, 200, (float) text_w, (float) text_h};

    TTF_GetTextSize(AI_TWO_text, &text_w, &text_h);
    SDL_FRect AI_TWO_rect = {(float) (SCREEN_WIDTH - text_w) / 2, 400, (float) text_w, (float) text_h};

    TTF_GetTextSize(NO_RANDOMNESS_text, &text_w, &text_h);
    SDL_FRect NO_RANDOMNESS_rect = {0, 300, (float) text_w, (float) text_h};

    TTF_GetTextSize(SOME_RANDOMNESS_text, &text_w, &text_h);
    SDL_FRect SOME_RANDOMNESS_rect = {(float) (SCREEN_WIDTH - text_w) / 2, 300, (float) text_w, (float) text_h};

    TTF_GetTextSize(ALL_RANDOMNESS_text, &text_w, &text_h);
    SDL_FRect ALL_RANDOMNESS_rect = {(float) (SCREEN_WIDTH - text_w), 300, (float) text_w, (float) text_h};

    TTF_GetTextSize(NO_RANDOMNESS2_text, &text_w, &text_h);
    SDL_FRect NO_RANDOMNESS2_rect = {0, 500, (float) text_w, (float) text_h};

    TTF_GetTextSize(SOME_RANDOMNESS2_text, &text_w, &text_h);
    SDL_FRect SOME_RANDOMNESS2_rect = {(float) (SCREEN_WIDTH - text_w) / 2, 500, (float) text_w, (float) text_h};

    TTF_GetTextSize(ALL_RANDOMNESS2_text, &text_w, &text_h);
    SDL_FRect ALL_RANDOMNESS2_rect = {(float) (SCREEN_WIDTH - text_w), 500, (float) text_w, (float) text_h};

    TTF_GetTextSize(NEXT_text, &text_w, &text_h);
    TTF_DrawRendererText(NEXT_text, SCREEN_WIDTH - text_w, SCREEN_HEIGHT - text_h);
    SDL_FRect NEXT_rect = {SCREEN_WIDTH - text_w, SCREEN_HEIGHT - text_h, (float) text_w, (float) text_h};
    SDL_FPoint mouse;
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
            case ACHI_PREGAME_AVA: TTF_GetTextSize(TITLE_text, &text_w, &text_h);
                TTF_DrawRendererText(TITLE_text, (float) (SCREEN_WIDTH - text_w) / 2, 0);
                TTF_DrawRendererText(BACK_text, 0, BACK_rect.y);
                TTF_DrawRendererText(ENTROPY_text, ENTROPY_rect.x, ENTROPY_rect.y);
                TTF_DrawRendererText(AI_ONE_text, AI_ONE_rect.x, AI_ONE_rect.y);
                TTF_DrawRendererText(AI_TWO_text, AI_TWO_rect.x, AI_TWO_rect.y);
                TTF_DrawRendererText(NO_RANDOMNESS_text, NO_RANDOMNESS_rect.x, NO_RANDOMNESS_rect.y);
                TTF_DrawRendererText(SOME_RANDOMNESS_text, SOME_RANDOMNESS_rect.x, SOME_RANDOMNESS_rect.y);
                TTF_DrawRendererText(ALL_RANDOMNESS_text, ALL_RANDOMNESS_rect.x, ALL_RANDOMNESS_rect.y);
                TTF_DrawRendererText(NO_RANDOMNESS2_text, NO_RANDOMNESS2_rect.x, NO_RANDOMNESS2_rect.y);
                TTF_DrawRendererText(SOME_RANDOMNESS2_text, SOME_RANDOMNESS2_rect.x, SOME_RANDOMNESS2_rect.y);
                TTF_DrawRendererText(ALL_RANDOMNESS2_text, ALL_RANDOMNESS2_rect.x, ALL_RANDOMNESS2_rect.y);
                if (SDL_PollEvent(&event)) {
                    switch (event.type) {
                        case SDL_EVENT_QUIT: quit = true;
                            break;
                        case SDL_EVENT_MOUSE_BUTTON_UP:
                            if (SDL_PointInRectFloat(&mouse, &BACK_rect)) {
                                scene = ACHI_PREGAME_ROUNDS;
                            }
                            if (SDL_PointInRectFloat(&mouse, &NO_RANDOMNESS_rect)) {
                                order_selected = true;
                                randomness_first_ai = NO_RAND;
                                TTF_SetTextColorFloat(NO_RANDOMNESS_text, 0xFF, 0x00, 0x00,SDL_ALPHA_OPAQUE_FLOAT);
                                TTF_SetTextColorFloat(SOME_RANDOMNESS_text, 0xFF, 0xFF, 0xFF,SDL_ALPHA_OPAQUE_FLOAT);
                                TTF_SetTextColorFloat(ALL_RANDOMNESS_text, 0xFF, 0xFF, 0xFF,SDL_ALPHA_OPAQUE_FLOAT);
                                strcpy(ai_desc,"Minimax");
                            }
                            if (SDL_PointInRectFloat(&mouse, &SOME_RANDOMNESS_rect)) {
                                order_selected = true;
                                randomness_first_ai = SOME_RAND;
                                TTF_SetTextColorFloat(SOME_RANDOMNESS_text, 0xFF, 0x00, 0x00,SDL_ALPHA_OPAQUE_FLOAT);
                                TTF_SetTextColorFloat(ALL_RANDOMNESS_text, 0xFF, 0xFF, 0xFF,SDL_ALPHA_OPAQUE_FLOAT);
                                TTF_SetTextColorFloat(NO_RANDOMNESS_text, 0xFF, 0xFF, 0xFF,SDL_ALPHA_OPAQUE_FLOAT);
                                strcpy(ai_desc,"Half-Random");
                            }
                            if (SDL_PointInRectFloat(&mouse, &ALL_RANDOMNESS_rect)) {
                                order_selected = true;
                                randomness_first_ai = ALL_RAND;
                                TTF_SetTextColorFloat(ALL_RANDOMNESS_text, 0xFF, 0x00, 0x00,SDL_ALPHA_OPAQUE_FLOAT);
                                TTF_SetTextColorFloat(SOME_RANDOMNESS_text, 0xFF, 0xFF, 0xFF,SDL_ALPHA_OPAQUE_FLOAT);
                                TTF_SetTextColorFloat(NO_RANDOMNESS_text, 0xFF, 0xFF, 0xFF,SDL_ALPHA_OPAQUE_FLOAT);
                                strcpy(ai_desc,"Random");
                            }
                            if (SDL_PointInRectFloat(&mouse, &NO_RANDOMNESS2_rect)) {
                                second_ai_rand_selected = true;
                                randomness_second_ai = NO_RAND;
                                TTF_SetTextColorFloat(NO_RANDOMNESS2_text, 0xFF, 0x00, 0x00,SDL_ALPHA_OPAQUE_FLOAT);
                                TTF_SetTextColorFloat(SOME_RANDOMNESS2_text, 0xFF, 0xFF, 0xFF,SDL_ALPHA_OPAQUE_FLOAT);
                                TTF_SetTextColorFloat(ALL_RANDOMNESS2_text, 0xFF, 0xFF, 0xFF,SDL_ALPHA_OPAQUE_FLOAT);
                                strcpy(ai2_desc,"Minimax");
                            }
                            if (SDL_PointInRectFloat(&mouse, &SOME_RANDOMNESS2_rect)) {
                                second_ai_rand_selected = true;
                                randomness_second_ai = SOME_RAND;
                                TTF_SetTextColorFloat(SOME_RANDOMNESS2_text, 0xFF, 0x00, 0x00,SDL_ALPHA_OPAQUE_FLOAT);
                                TTF_SetTextColorFloat(ALL_RANDOMNESS2_text, 0xFF, 0xFF, 0xFF,SDL_ALPHA_OPAQUE_FLOAT);
                                TTF_SetTextColorFloat(NO_RANDOMNESS2_text, 0xFF, 0xFF, 0xFF,SDL_ALPHA_OPAQUE_FLOAT);
                                strcpy(ai2_desc,"Half-Random");
                            }
                            if (SDL_PointInRectFloat(&mouse, &ALL_RANDOMNESS2_rect)) {
                                second_ai_rand_selected = true;
                                randomness_second_ai = ALL_RAND;
                                TTF_SetTextColorFloat(ALL_RANDOMNESS2_text, 0xFF, 0x00, 0x00,SDL_ALPHA_OPAQUE_FLOAT);
                                TTF_SetTextColorFloat(SOME_RANDOMNESS2_text, 0xFF, 0xFF, 0xFF,SDL_ALPHA_OPAQUE_FLOAT);
                                TTF_SetTextColorFloat(NO_RANDOMNESS2_text, 0xFF, 0xFF, 0xFF,SDL_ALPHA_OPAQUE_FLOAT);
                                strcpy(ai2_desc,"Random");
                            }
                            if (SDL_PointInRectFloat(&mouse, &NEXT_rect) && order_selected && second_ai_rand_selected) {
                                TTF_DestroyText(TITLE_text);
                                char buf2[1024];
                                SDL_Log("%s",ai2_desc);
                                sprintf(buf2,"%s AI VS %s AI Mode",ai_desc,ai2_desc);
                                TITLE_text = TTF_CreateText(text_engine,font,buf2,0);
                                scene = ACHI_GAME_START;
                            }
                            break;
                        case SDL_EVENT_KEY_DOWN:
                            if (event.key.key == SDLK_RETURN && order_selected) {
                                TTF_DestroyText(TITLE_text);
                                char buf2[1024];
                                SDL_Log("%s",ai2_desc);
                                sprintf(buf2,"%s AI VS %s AI Mode",ai_desc,ai2_desc);
                                TITLE_text = TTF_CreateText(text_engine,font,buf2,0);
                                scene = ACHI_GAME_START;
                            }
                            break;
                        default: ;
                    }
                }
                if (order_selected && second_ai_rand_selected) {
                    TTF_DrawRendererText(NEXT_text, NEXT_rect.x, NEXT_rect.y);
                    if (SDL_PointInRectFloat(&mouse, &NEXT_rect))
                        SDL_SetCursor(pointing_cursor);
                }
                if (SDL_PointInRectFloat(&mouse, &BACK_rect) || SDL_PointInRectFloat(&mouse, &ALL_RANDOMNESS_rect) ||
                    SDL_PointInRectFloat(&mouse, &SOME_RANDOMNESS_rect) || SDL_PointInRectFloat(
                        &mouse, &NO_RANDOMNESS_rect) || SDL_PointInRectFloat(&mouse, &ALL_RANDOMNESS2_rect) ||
                    SDL_PointInRectFloat(&mouse, &SOME_RANDOMNESS2_rect) || SDL_PointInRectFloat(
                        &mouse, &NO_RANDOMNESS2_rect))
                    SDL_SetCursor(pointing_cursor);
                break;

            case ACHI_GAME_START:
                if (round <= max_rounds) {
                    int turn = (round % 2 != 0) ? 1 : 2;
                    TTF_GetTextSize(TITLE_text, &text_w, &text_h);
                    TTF_DrawRendererText(TITLE_text, (float) (SCREEN_WIDTH - text_w) / 2, 0);
                    if (ROUND_text != nullptr)
                        TTF_DestroyText(ROUND_text);
                    if (game_board == nullptr) {
                        game_board = create_board();
                        board_cleaner = game_board;
                    }
                    if (is_winning(game_board) || round > max_rounds) {
                        state = is_winning(game_board);
                        scene = ACHI_END;
                    }

                    switch (game_mode) {
                        case GAME_MODE_PVA:
                            if (ai_first)
                                sprintf(buf, "Round N°%d - %s Phase - %s", round, (round <= 6) ? "Placement" : "Moving",
                                        (turn == 1) ? "AI's turn" : "Players turns");
                            else
                                sprintf(buf, "Round N°%d - %s Phase - %s", round, (round <= 6) ? "Placement" : "Moving",
                                        (turn == 2) ? "AI's turn" : "Players turns");
                            break;
                        case GAME_MODE_PVP:
                            sprintf(buf, "Round N°%d - %s Phase - Player %d turn", round,
                                    (round <= 6) ? "Placement" : "Moving", turn);
                            break;
                        case GAME_MODE_AVA:
                            sprintf(buf, "Round N°%d - %s turn Phase - %s", round, (round <= 6) ? "Placement" : "Moving",
                                    (round % 2 != 0) ? ai_desc : ai2_desc);
                        default: ;
                    }
                    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00,SDL_ALPHA_OPAQUE_FLOAT);
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

                                if (game_mode == GAME_MODE_PVP) {
                                    if (round <= 6) {
                                        for (int i = 0; i < 9; ++i) {
                                            if (game_board[i].occupied_by == 0 &&
                                                SDL_PointInRectFloat(&mouse, &squares[i])) {
                                                game_board = next_board(game_board, i, round++);
                                                free(board_cleaner);
                                                board_cleaner = game_board;
                                                break;
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
                                                        selected_index = j;
                                                        break;
                                                    }
                                                }
                                                get_adjacent(game_board, &number_adjacent, selected, adjacent_squares);
                                                for (int j = 0; j < number_adjacent; ++j) {
                                                    int current_adjacent = (selected == 4)
                                                                               ? adjacent_to_center[adjacent_squares[j]]
                                                                               : adjacencyMatrix2[selected][
                                                                                   adjacent_squares
                                                                                   [j]];
                                                    if (SDL_PointInRectFloat(&mouse, &squares[current_adjacent])) {
                                                        selected = -1;
                                                        game_board =
                                                                next_board(game_board, selected_index * 3 + j, round++);
                                                        free(board_cleaner);
                                                        board_cleaner = game_board;
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                if (game_mode == GAME_MODE_PVA) {
                                    if (turn == ((ai_first) ? 2 : 1)) {
                                        if (round <= 6) {
                                            for (int i = 0; i < 9; ++i) {
                                                if (game_board[i].occupied_by == 0 &&
                                                    SDL_PointInRectFloat(&mouse, &squares[i])) {
                                                    game_board = next_board(game_board, i, round++);
                                                    free(board_cleaner);
                                                    board_cleaner = game_board;
                                                    break;
                                                }
                                            }
                                        } else {
                                            for (int i = 0; i < 9; ++i) {
                                                if ((game_board[i].occupied_by == ((ai_first) ? -1 : 1)) &&
                                                    SDL_PointInRectFloat(&mouse, &squares[i])) {
                                                    selected = i;
                                                }
                                                if (selected != -1) {
                                                    int number_played = 0;
                                                    int player_squares[3] = {-1, -1, -1};
                                                    int number_adjacent = 0;
                                                    int adjacent_squares[3] = {-1, -1, -1};
                                                    int selected_index;
                                                    get_played(game_board, &number_played, (ai_first) ? -1 : 1,
                                                               player_squares);
                                                    for (int j = 0; j < number_played; ++j) {
                                                        if (selected == player_squares[j]) {
                                                            selected_index = j;
                                                            break;
                                                        }
                                                    }
                                                    get_adjacent(game_board, &number_adjacent, selected,
                                                                 adjacent_squares);
                                                    for (int j = 0; j < number_adjacent; ++j) {
                                                        int current_adjacent = (selected == 4)
                                                                                   ? adjacent_to_center[adjacent_squares
                                                                                       [j]]
                                                                                   : adjacencyMatrix2[selected][
                                                                                       adjacent_squares
                                                                                       [j]];
                                                        if (SDL_PointInRectFloat(&mouse, &squares[current_adjacent])) {
                                                            selected = -1;
                                                                game_board =
                                                                        next_board(game_board, selected_index * 3 + j,
                                                                                   round++);
                                                                free(board_cleaner);
                                                                board_cleaner = game_board;
                                                            break;
                                                        }
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
                    if (selected != -1 && game_board[selected].occupied_by != ((turn == 2) ? -1 : turn)) {
                        selected = -1;
                    }
                    if (game_mode == GAME_MODE_PVA) {
                        if (skip_cycle && state == 0) {
                            if (turn == ((ai_first) ? 1 : 2)) {
                                TTF_DrawRendererText(AI_IS_THINKING_text, AI_THINKING_x, AI_THINKING_y);
                                SDL_RenderPresent(renderer);
                                SDL_Delay(200);
                                pair place = minimax(game_board, ai_first, round,
                                                     SDL_min(max_rounds + 1,
                                                             (DLS_LIMIT +(round / DLS_LIMIT) * DLS_LIMIT) +1));
                                game_board =
                                        next_board(game_board, place.best_move, round++);
                                free(board_cleaner);
                                board_cleaner = game_board;
                            }
                            skip_cycle = false;
                        } else {
                            skip_cycle = true;
                        }
                    }
                    if (game_mode == GAME_MODE_AVA) {
                        if (skip_cycle) {
                            int placement;
                            board temp_board = nullptr;
                            pair place;
                            RANDOMNESS current_randomness = (turn == 1) ? randomness_first_ai : randomness_second_ai;
                            switch (current_randomness) {
                                case NO_RAND:
                                    if (turn == 1) {
                                        TTF_DrawRendererText(AI_IS_THINKING_text, AI_THINKING_x, AI_THINKING_y);
                                        SDL_RenderPresent(renderer);
                                        SDL_Delay(200);
                                        place = minimax(game_board, true, round,
                                                        SDL_min(max_rounds + 1,
                                                                (DLS_LIMIT+(round / DLS_LIMIT) * DLS_LIMIT) +1));
                                        placement = place.best_move;
                                        temp_board = next_board(game_board, placement, round++);
                                    } else {
                                        TTF_DrawRendererText(AI_IS_THINKING_text, AI_THINKING_x, AI_THINKING_y);
                                        SDL_RenderPresent(renderer);
                                        SDL_Delay(200);
                                        place = minimax(game_board, false, round,
                                                        SDL_min(max_rounds + 1,
                                                                (DLS_LIMIT+(round / DLS_LIMIT) * DLS_LIMIT) +1));
                                        placement = place.best_move;
                                        temp_board = next_board(game_board, placement, round++);
                                    }
                                SDL_Log("Minimax Move");
                                    break;
                                case SOME_RAND:
#ifdef _SDL_PLATFORM_WINDOWS
                                    if (rand() % 11 == 2) {
                                        do {
                                            placement = (int) rand() % 10;
#else
                                    if (arc4random_uniform(10) == 2) {
                                        do {
                                            placement = (int) arc4random_uniform(9);
#endif

                                            temp_board =
                                                    next_board(game_board, placement, round);
                                        } while (temp_board == nullptr);
                                        round++;
                                    } else {
                                        if (turn == 1) {
                                            TTF_DrawRendererText(AI_IS_THINKING_text, AI_THINKING_x, AI_THINKING_y);
                                            SDL_RenderPresent(renderer);
                                            SDL_Delay(200);
                                            place = minimax(game_board, true, round,
                                                            SDL_min(max_rounds + 1,
                                                                    (DLS_LIMIT+(round / DLS_LIMIT) * DLS_LIMIT) +1));
                                            placement = place.best_move;
                                            temp_board = next_board(game_board, placement, round++);
                                        } else {
                                            TTF_DrawRendererText(AI_IS_THINKING_text, AI_THINKING_x, AI_THINKING_y);
                                            SDL_RenderPresent(renderer);
                                            SDL_Delay(200);
                                            place = minimax(game_board, false, round,
                                                            SDL_min(max_rounds + 1,
                                                                    (DLS_LIMIT+(round / DLS_LIMIT) * DLS_LIMIT) +1));
                                            placement = place.best_move;
                                            temp_board = next_board(game_board, placement, round++);
                                        }
                                    }
                                    break;
                                case ALL_RAND: do {
#ifdef SDL_PLATFORM_WINDOWS
                                        placement = (int) rand() % 10;
#else
                                        placement = (int) arc4random_uniform(9);
#endif
                                        temp_board =
                                                next_board(game_board, placement, round);
                                        SDL_Log("Random Move");
                                    } while (temp_board == nullptr);
                                    round++;
                                    SDL_Delay(200);
                                    break;
                            }

                            game_board = temp_board;
                            free(board_cleaner);
                            board_cleaner = game_board;
                            skip_cycle = false;
                        } else {
                            skip_cycle = true;
                        }
                    }
                } else {
                    scene = ACHI_END;
                }
                break;
            case ACHI_END:
                if (quit != true) {
                    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00,SDL_ALPHA_OPAQUE_FLOAT);
                    if (SDL_PointInRectFloat(&mouse, &RETRY_rect) ||
                        SDL_PointInRectFloat(&mouse, &MAIN_MENU_rect))
                        SDL_SetCursor(pointing_cursor);
                    switch (game_mode) {
                        case GAME_MODE_PVP:
                            if (state != 0)
                                sprintf(buf, "N°%d Player %d Wins - Play Again ?", round,
                                        state == -1 ? 2 : 1);
                            else
                                sprintf(buf, "N°%d Tie - Play Again ?", round);
                            break;
                        case GAME_MODE_PVA:
                            if (ai_first) {
                                if (state == 1)
                                    sprintf(buf, "N°%d AI Wins - Play Again ?", round);
                                else if (state == -1)
                                    sprintf(buf, "N°%d Player Wins - Play Again ?", round);
                                else
                                    sprintf(buf, "N°%d Tie - Play Again ?", round);
                            } else {
                                if (state == -1)
                                    sprintf(buf, "N°%d AI Wins - Play Again ?", round);
                                else if (state == 1)
                                    sprintf(buf, "N°%d Player Wins - Play Again ?", round);
                                else
                                    sprintf(buf, "N°%d Tie - Play Again ?", round);
                            }
                            break;
                        case GAME_MODE_AVA:
                            if (state == 1)
                                sprintf(buf, "N°%d AI 1 Wins - Play Again ?", round);
                            else if (state == -1)
                                sprintf(buf, "N°%d AI 2 Wins - Play Again ?", round);
                            else
                                sprintf(buf, "N°%d Tie - Play Again ?", round);
                        default: ;
                    }
                    TTF_DestroyText(ROUND_text);
                    ROUND_text = TTF_CreateText(text_engine, font, buf, 0);
                    TTF_GetTextSize(ROUND_text, &text_w, &text_h);
                    TTF_DrawRendererText(ROUND_text, (float) (SCREEN_WIDTH - text_w) / 2, 75);
                    TTF_DrawRendererText(RETRY_text, RETRY_rect.x, RETRY_rect.y);
                    TTF_DrawRendererText(MAIN_MENU_text, MAIN_MENU_rect.x, MAIN_MENU_rect.y);
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
                                squares[i].w = squares[i].h = PLAYER_SIZE;
                                SDL_SetTextureColorModFloat(player_occupied, 0xcf, 0xff, 0xdd);
                                SDL_RenderTexture(renderer, player_occupied, nullptr, &squares[i]);
                                break;
                            case 0:
                                squares[i].x = hot_points[i].x - (float) EMPTY_SIZE / 2;
                                squares[i].y = hot_points[i].y - (float) EMPTY_SIZE / 2;
                                squares[i].w = squares[i].h = EMPTY_SIZE;
                                SDL_SetTextureColorModFloat(unoccupied_square, 0xFF, 0xff, 0xFF);
                                SDL_RenderTexture(renderer, unoccupied_square, nullptr, &squares[i]);
                                break;
                            case 1:
                                squares[i].x = hot_points[i].x - (float) PLAYER_SIZE / 2;
                                squares[i].y = hot_points[i].y - (float) PLAYER_SIZE / 2;
                                squares[i].w = squares[i].h = PLAYER_SIZE;
                                squares[i].w = squares[i].h = PLAYER_SIZE;
                                SDL_SetTextureColorModFloat(player_occupied, 0xd0, 0xba, 0xff);
                                SDL_RenderTexture(renderer, player_occupied, nullptr, &squares[i]);
                                break;
                            default: ;
                        }
                    }

                    if (SDL_PollEvent(&event)) {
                        switch (event.type) {
                            case SDL_EVENT_QUIT: SDL_Log("Quitting");
                                quit = true;
                                break;
                            case SDL_EVENT_MOUSE_BUTTON_UP:
                                if (event.button.button == SDL_BUTTON_LEFT) {
                                    if (SDL_PointInRectFloat(&mouse, &RETRY_rect)) {
                                        buf[0] = '\0';
                                        round = 1;
                                        selected = -1;
                                        state = 0;
                                        if (game_board != nullptr) {
                                            free(game_board);
                                            game_board = board_cleaner = nullptr;
                                        }

                                        scene = ACHI_GAME_START;
                                    }
                                    if (SDL_PointInRectFloat(&mouse, &MAIN_MENU_rect)) {
                                        buf[0] = '\0';
                                        round = 1;
                                        selected = -1;
                                        state = 0;
                                        if (game_board != nullptr) {
                                            free(game_board);
                                            game_board = board_cleaner = nullptr;
                                        }

                                        game_mode = NONE;
                                        scene = ACHI_MENU;
                                    }
                                }
                            default: ;
                        }
                    }
                }
                break;
            case ACHI_ABOUT:
                TTF_DrawRendererText(ABOUT_TITLE_text, ABOUT_TITLE_x, 0);
                TTF_DrawRendererText(BACK_text, BACK_rect.x, BACK_rect.y);
                SDL_RenderTexture(renderer, CREDITS_text_texture, nullptr, &CREDITS_rect);
                if (SDL_PointInRectFloat(&mouse, &BACK_rect))
                    SDL_SetCursor(pointing_cursor);
                if (SDL_PollEvent(&event)) {
                    switch (event.type) {
                        case SDL_EVENT_QUIT: quit = true;
                            break;
                        case SDL_EVENT_MOUSE_BUTTON_UP:
                            if (SDL_PointInRectFloat(&mouse, &BACK_rect)) {
                                scene = ACHI_MENU;
                            }
                            break;
                        default: ;
                    }
                }
                break;
        }
        SDL_RenderPresent(renderer);
    }
    free(game_board);
    TTF_CloseFont(font);
    TTF_CloseFont(font_credits);
    TTF_CloseFont(font_underlined);
    TTF_Quit();
    SDL_DestroyCursor(default_cursor);
    SDL_DestroyCursor(pointing_cursor);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
