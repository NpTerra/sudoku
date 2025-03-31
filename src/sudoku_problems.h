//
// Created by User on 22/02/2024.
//

#ifndef SUDOKU_PROBLEMS_H
#define SUDOKU_PROBLEMS_H

#include <stack>
#include <vector>
#include <tuple>
#include "game_objects.h"

enum Problem {
    A_LEGJOBB = 0,
    RANDOM = 666
};

void set_map(std::stack<Map> &mapbuffer, std::pair<int, int> &prev, Problem p);

#endif //SUDOKU_PROBLEMS_H
