//
// Created by User on 22/02/2024.
//

#include "game_objects.h"

bool check_map(Map& map)
{
    for(int i = 0, j, check; i < 9; i++)
    {
        for(j = 0, check = 0; j < 9; j++)
        {
            // horizontal || vertical || zone
            if(    map.tiles[i][j].collapsed == -1
                || map.tiles[j][i].collapsed == -1
                || map.tiles[(i/3*3)+(i%3)][(j/3*3)+(j%3)].collapsed == -1)
                return false;

            check |= 1 << map.tiles[i][j].collapsed;                               // 000000000x000000000x111111111x
            check |= 1024 << map.tiles[j][i].collapsed;                            // 000000000x111111111x000000000x
            check |= 1048576 << map.tiles[(i/3*3)+(i%3)][(j/3*3)+(j%3)].collapsed; // 111111111x000000000x000000000x
            // 111111111011111111101111111110 = 1072692222 = CORRECT_LINE
        }

        if(check != CORRECT_LINE)
            return false;
    }
    return true;
}