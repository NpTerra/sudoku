#include "sudoku_problems.h"

std::vector<std::vector<std::tuple<int, int, int>>> problems = {
    {
        {0, 0, 8},
    {2, 1, 3},
    {3, 1, 6},
    {1, 2, 7},
    {4, 2, 9},
    {6, 2, 2},
    {1, 3, 5},
    {5, 3, 7},
    {4, 4, 4},
    {5, 4, 5},
    {6, 4, 7},
    {3, 5, 1},
    {7, 5, 3},
    {2, 6, 1},
    {7, 6, 6},
    {8, 6, 8},
    {2, 7, 8},
    {3, 7, 5},
    {7, 7, 1},
    {1, 8, 9},
    {6, 8, 4}
    }
};

void set_map(std::stack<Map> &mapbuffer, std::pair<int, int> &prev, Problem p) {
    while(!mapbuffer.empty())
        mapbuffer.pop();
    mapbuffer.emplace();
    prev = {-1, -1};

    if(p == RANDOM) {
        return;
    }

    for(auto &[i, j, n] : problems[p]) {
        mapbuffer.top().updateAtPos(i, j, n, true, false);
    }
}