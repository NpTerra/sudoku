#ifndef SUDOKU_MAP_H
#define SUDOKU_MAP_H

#define CORRECT_LINE 1072692222

#include <unordered_set>
#include <vector>
#include <queue>
#include <stack>

struct Tile {
    std::unordered_set<int> s_pos = {1,2,3,4,5,6,7,8,9};
    bool predefined = false;
    int collapsed = -1;

    void set(int n, bool pred)
    {
        this->predefined = pred;
        this->collapsed = n;
        this->s_pos.clear();
        this->s_pos.emplace(n);
    }
};

struct Map {
    std::vector< std::vector<Tile> > tiles;
    int next = -1;
    int nextval = -1;

    Map() {
        tiles = std::vector(9, std::vector(9, Tile()));
    }

    Map(Map const &parent) {
        tiles = parent.tiles;
    }

    Tile& operator[](const std::pair<int,int> &n)
    {
        return this->tiles[n.first][n.second];
    }

    Tile& operator[](const int &n)
    {
        return this->tiles[n/9][n%9];
    }

    void resetTilePositions() {
        std::queue<std::pair<int, int>> coll;
        for(int i = 0; i < 9; i++) {
            for(int j = 0; j < 9; j++) {
                if(this->tiles[i][j].collapsed != -1) {
                    coll.emplace(i,j);
                    continue;
                }

                this->tiles[i][j] = Tile();
            }
        }
        while(!coll.empty())
        {
            std::pair<int, int> &c = coll.front();
            if(this->tiles[c.first][c.second].collapsed == -1) continue;

            for(int x = 0; x < 9; x++) {
                this->tiles[x][c.second].s_pos.erase(this->tiles[c.first][c.second].collapsed);
                this->tiles[c.first][x].s_pos.erase(this->tiles[c.first][c.second].collapsed);
                this->tiles[(c.first/3*3)+(x/3)][(c.second/3*3)+(x%3)].s_pos.erase(this->tiles[c.first][c.second].collapsed);
            }
            coll.pop();
        }
    }

    void updateAtPos(int i, int j, int n, bool pred, bool reset)
    {
        this->tiles[i][j].set(n, pred);
        if(reset) {
            resetTilePositions();
        }
        else {
            for(int x = 0; x < 9; x++) {
                this->tiles[x][j].s_pos.erase(n);
                this->tiles[i][x].s_pos.erase(n);
                this->tiles[(i/3*3)+(x/3)][(j/3*3)+(x%3)].s_pos.erase(n);
            }
        }
    }

    void updateAtIndex(int i, int n, bool pred, bool reset) {
        int j = i%9;
        i /= 9;
        this->updateAtPos(i, j, n, pred, reset);
    }
};

bool check_map(Map& map);

#endif //SUDOKU_MAP_H
