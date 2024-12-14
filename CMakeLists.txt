cmake_minimum_required(VERSION 3.30)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -g -O1")
project(Achi C)
set(CMAKE_C_STANDARD 23)
include(FetchContent)
FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL
        GIT_TAG main
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SDL3)
FetchContent_Declare(
        SDL3_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf
        GIT_TAG main
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SDL3_ttf)
FetchContent_Declare(
        SDL3_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image
        GIT_TAG main
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SDL3_image)
add_executable(Achi-SDL main.c src/decision_tree.c src/game_board.c src/game_interface.c)
add_executable(Achi-console main-con.c src/decision_tree.c src/game_board.c src/game_interface.c)
target_include_directories(Achi-SDL PRIVATE include)
target_link_libraries(Achi-SDL SDL3 SDL3_ttf SDL3_image)
target_include_directories(Achi-console PRIVATE include)