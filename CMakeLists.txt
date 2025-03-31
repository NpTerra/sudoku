cmake_minimum_required(VERSION 3.25)
project(sudoku)

set(CMAKE_CXX_STANDARD 17)

include_directories(tools)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/styles/pumpkin.h
        src/sudoku_problems.cpp
        src/game_objects.cpp
        src/game_objects.h
        src/sudoku_problems.h
)

set(RAYLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/raylib-4.5.0_win64_mingw-w64)
include_directories(${RAYLIB_DIR}/include)
target_link_libraries(${PROJECT_NAME} ${RAYLIB_DIR}/lib/libraylibdll.a)
