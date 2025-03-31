#include <iostream>
#include <vector>
#include <stack>
#include <chrono>
#include <queue>

#include "raypp/raylib-cpp.hpp"
#include "game_objects.h"
#include "sudoku_problems.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#undef RAYGUI_IMPLEMENTATION

#define MAX_GUI_STYLES_AVAILABLE   2
#include "styles/pumpkin.h"

#define NAME "Sudoku"
#define VERSION "1.0-Beta"

const char* pops[] = {"Warning!", "Error!", "Success!", "Info."};
const char* nums[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};

std::vector<std::vector<std::pair<int,int>>> groups(9);

long long getTime() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch()).count();
}

enum Popup {
    WARNING = 0,
    ERROR = 1,
    SUCCESS = 2,
    INFO = 3
};

inline void create_snapshot(std::stack<Map>& buffer, std::pair<int, int>& prev)
{
    buffer.emplace(buffer.top());
    prev = {-1, -1};
}


/*
 * returns false if there is no other snapshot to step back to.
 * */
inline bool stepback_snapshot(std::stack<Map>& buffer, std::pair<int, int>& prev)
{
    prev = {-1, -1};

    if (buffer.size() > 1) {
        buffer.pop();
        return true;
    }

    return false;
}

float padding = 15;
float boxw = 486;
float dh = 150;
float menu_margin = padding * 2 + boxw;
long long solving_time = -1;
int starting_snapshot = -1;

float screen_width = (padding * 3) + (boxw * 2);
float screen_height = menu_margin;

bool is_gui_enabled = true;
bool solving = false;

bool take_user_input = true;
bool edit_mode = false;
bool auto_candidate = false;
bool popup_window = false;
std::string popup_text;
int popup_title = -1;

void popup(Popup type, const char* text)
{
    popup_window = true;
    is_gui_enabled = false;
    popup_text = text;
    popup_title = type;
}

int main()
{
    raylib::Window window(screen_width, screen_height+1, TextFormat("%s v%s", NAME, VERSION));
    GuiLoadStylePumpkin();

    raylib::Color bg = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
    raylib::Color fg = GetColor(GuiGetStyle(DEFAULT, LINE_COLOR));
    raylib::Color ln = raylib::Color(55, 38, 41);

    std::stack<Map> mapbuffer;
    mapbuffer.emplace();

    std::pair<int, int> prev = {-1, -1};
    std::pair<int, int> select;
    std::vector<int> disabled(81, 0);
    std::vector<int> values(81, -1);
    bool selected;
    bool temp;

    set_map(mapbuffer, prev, A_LEGJOBB);

    SetTargetFPS(60);

    while (!window.ShouldClose()) {
        // ----------
        // Solver
        // ----------
        if(solving && solving_time == -1) {
            solving_time = getTime();
        }

        while(solving) {
            int next_selected;
            int a = 10;
            int b = -1;

            for(int i = 0; i < 81; i++) {
                if(mapbuffer.top()[i].collapsed == -1) {
                    if(mapbuffer.top()[i].s_pos.size() < a) {
                        a = mapbuffer.top()[i].s_pos.size();
                        next_selected = i;
                    }
                }
            }

            if(a == 0) // wrong answer to the puzzle
            {
                if(mapbuffer.size() == starting_snapshot) { // no snapshot to step back to
                    solving = false;
                    take_user_input = true;
                    solving_time = -1;
                    popup(ERROR, "There is no answer to the given puzzle!");
                }
                else {
                    stepback_snapshot(mapbuffer, prev);
                    auto &x = mapbuffer.top();
                    x[x.next].s_pos.erase(x.nextval);
                }
            }
            else if(a == 10) { // answer found
                solving = false;
                take_user_input = true;
                solving_time = getTime() - solving_time;
                int ms = solving_time%1000; solving_time /= 1000;
                int sec = solving_time%60; solving_time /= 60;
                int min = solving_time%60; solving_time /= 60;
                int h = solving_time;

                solving_time = -1;
                popup(SUCCESS, TextFormat("Puzzle Solved! Solving time: %dh %dmin %dsec %dms", h, min, sec, ms));
            }
            else {
                auto &x = mapbuffer.top();
                b = *x[next_selected].s_pos.begin();
                x.next = next_selected;
                x.nextval = b;
                create_snapshot(mapbuffer, prev);
                mapbuffer.top().updateAtIndex(next_selected, b, false, false);
            }
        }





        // Window start
        window.BeginDrawing();
        window.ClearBackground(bg);





        // ----------
        // Grids
        // ----------

        GuiGrid({padding, padding, boxw, boxw}, "", 81*2, 3);

        GuiFade(0.7f);

        GuiGroupBox({menu_margin, padding, boxw, boxw+1}, "Settings Menu");

        GuiFade(0.3f);

        GuiGroupBox({menu_margin + boxw/2 + padding*7, padding*2, boxw/2 - padding*8, padding*17}, "Debug Info");
        GuiGroupBox({menu_margin + boxw/2 + padding*7, screen_height-padding*14, boxw/2 - padding*8, padding*12}, "Table Controls");

        GuiGroupBox({menu_margin + padding, screen_height-padding*12-boxw/2, boxw/2 + padding*5 + 1, boxw/2 + padding*10}, "Tile Controls");

        GuiFade(1.0f);

        DrawText("SUDOKU", menu_margin + padding*3, padding*2-4, 64, fg);

        GuiGrid({menu_margin+padding*2, screen_height-padding*9-boxw/2, boxw/2 + padding*3, boxw/2 + padding*3}, "", 96, 1);

        if(!is_gui_enabled)
            GuiLock();










        // ----------
        // Sudoku Table
        // ----------

        for(int i = 0, j; i < 9; i++)
        {
            for(j = 0; j < 9; j++)
            {
                Tile& curr = mapbuffer.top().tiles[i][j];

                if(curr.predefined)
                    GuiDisable();
                selected = (prev.first == i && prev.second == j);

                if (GuiToggle({padding + 2 + j * 54.0f, padding + 2 + i * 54.0f, 51, 51},
                              curr.collapsed >= 0 || curr.predefined ? std::to_string(curr.collapsed).c_str() : "", selected))
                {

                    if (!selected && take_user_input)
                    {
                        prev.first = i;
                        prev.second = j;
                    }
                }
                else if(selected && take_user_input)
                {
                    prev = {-1, -1};
                }
                if(curr.collapsed == -1 && auto_candidate) {
                    //x: padding + 2 + j * 54.0f
                    //y: padding + 2 + i * 54.0f

                    for(auto &x : curr.s_pos) {
                        //TraceLog(LOG_INFO, TextFormat("%d", x));
                        DrawText(nums[x-1], padding + 6 + j * 54.0f + ((x-1)%3)*18,padding + 5 + i * 54.0f + ((x-1)/3)*18, 1, ln);
                        //GuiLabel({,5,5}, nums[x-1]);
                    }
                }

                if(curr.predefined)
                    GuiEnable();
            }
        }










        // ----------
        // Input UI
        // ----------

        if(!take_user_input) // Disable interactive components while solving the puzzle
            GuiDisable();

        if(prev.first == -1 and prev.second == -1)
            GuiDisable();

        for(int i = 0, j, ij; i < 3; i++)
        {
            for(j = 0; j < 3; j++)
            {
                ij = i * 3 + j;

                if(GuiButton({menu_margin+padding*2 + 5 + j*96,screen_height-padding*9-boxw/2 + 5 + i*96,86,86}, std::to_string(ij+1).c_str()))
                {
                    mapbuffer.top().updateAtPos(select.first, select.second, ij+1, false, true);
                }
            }
        }

        if(prev.first == -1 and prev.second == -1 && take_user_input)
            GuiEnable();

        auto_candidate = GuiCheckBox({menu_margin + padding*2, padding * 7+5, 15, 15}, "Auto Candidate Mode", auto_candidate);

        temp = edit_mode;
        edit_mode = GuiCheckBox({menu_margin + boxw/2 + padding*8, screen_height - (padding * 13), 15, 15}, "Edit Mode", edit_mode);

        if(temp != edit_mode) {
            prev = {-1, -1};

            for (auto& x: mapbuffer.top().tiles) {
                for (auto& y: x) {
                    if(y.collapsed > -1)
                        y.predefined = !edit_mode;
                }
            }
        }

        if(edit_mode)
            GuiDisable();

        if(GuiButton({menu_margin + padding*2, screen_height - (padding * 5), boxw/2 + padding*3 + 1, 30}, "Check Answer"))
        {
            if(check_map(mapbuffer.top()))
                popup(SUCCESS, "The puzzle is solved!");
            else
                popup(ERROR, "Incorrect answer!");
        }

        if(GuiButton({menu_margin + boxw/2 + padding*8, screen_height - (padding * 5), boxw/2 - padding*10, 30}, "Solve"))
        {
            if(take_user_input)
            {
                starting_snapshot = mapbuffer.size();
                take_user_input = false;
                solving = true;
            }
        }

        if(!edit_mode)
            GuiDisable();
        else
            GuiEnable();

        if(GuiButton({menu_margin + boxw/2 + padding*8, screen_height - (padding * 8), boxw/2 - padding*10, 30}, "Rollback"))
        {
            if(mapbuffer.size() > 1)
                mapbuffer.pop();
            else
                popup(WARNING, "There is no remaining snapshot to roll back to.");
        }

        if(GuiButton({menu_margin + boxw/2 + padding*8, screen_height - (padding * 11), boxw/2 - padding*10, 30}, "Clear"))
        {
            while(!mapbuffer.empty())
                mapbuffer.pop();
            mapbuffer.emplace();
            prev = {-1, -1};
        }

        if(!edit_mode)
            GuiEnable();

        if(!take_user_input) // Re-Enable components
            GuiEnable();

        GuiLabel({menu_margin + boxw/2 + padding*8, padding*2, 100, 50}, std::string("Selected: ").append(prev.first == -1 ? "-1" : std::to_string(prev.first*9 + prev.second)).c_str());
        GuiLabel({menu_margin + boxw/2 + padding*8, padding*3, 100, 50}, std::string("Snapshot: ").append(std::to_string(mapbuffer.size())).c_str());

        if(prev.first != -1 && prev.second != -1)
            GuiLabel({menu_margin + boxw/2 + padding*8, padding*4, 100, 50},
                     std::string("Tile val: ").append(std::to_string(mapbuffer.top().tiles[select.first][select.second].collapsed)).c_str());
        else
            GuiLabel({menu_margin + boxw/2 + padding*8, padding*4, 100, 50}, "No Tile");

        if(!is_gui_enabled)
            GuiUnlock();










        // ----------
        // Warning Windows
        // ----------

        if(popup_window)
        {
            GuiFade(0.5f);
            GuiDummyRec({0,0,screen_width, screen_height}, "IDK");
            GuiFade(1.0f);
            if(GuiWindowBox({200, 100, screen_width-400.0f, screen_height-200.0f}, pops[popup_title]))
            {
                popup_window = false;
                is_gui_enabled = true;
            }
            GuiLabel({205, 110, MeasureText(popup_text.c_str(), 13.0f) * 1.0f, 50}, popup_text.c_str());
        }

        window.EndDrawing();










        // ----------
        // Controls
        // ----------

        if(IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_DELETE))
        {
            if(!mapbuffer.top()[select].predefined)
            {
                mapbuffer.top().tiles[select.first][select.second] = Tile();
                mapbuffer.top().resetTilePositions();
            }
        }

        if(prev.first != -1 && prev.second != -1)
        {
            int i;
            if(IsKeyPressed(KEY_UP))
            {
                i = 0;
                do {
                    prev.first -= 1;
                    if(prev.first < 0) prev.first = 8;
                    select = {prev.first,prev.second};
                }
                while(mapbuffer.top()[select].predefined && (++i < 9));
            }
            if(IsKeyPressed(KEY_DOWN))
            {
                i = 0;
                do {
                    prev.first += 1;
                    if(prev.first >= 9) prev.first = 0;
                    select = {prev.first,prev.second};
                }
                while(mapbuffer.top()[select].predefined && (++i < 9));
            }
            if(IsKeyPressed(KEY_LEFT))
            {
                i = 0;
                do {
                    prev.second -= 1;
                    if(prev.second < 0) prev.second = 8;
                    select = {prev.first,prev.second};
                }
                while(mapbuffer.top()[select].predefined && (++i < 9));
            }
            if(IsKeyPressed(KEY_RIGHT))
            {
                i = 0;
                do {
                    prev.second += 1;
                    if(prev.second >= 9) prev.second = 0;
                    select = {prev.first,prev.second};
                }
                while(mapbuffer.top()[select].predefined && (++i < 9));
            }
            select = {prev.first,prev.second};
        }

        for (int i = 0; i < 9 && (prev.first != -1 && prev.second != -1); i++)
        {
            if (IsKeyPressed(KEY_ONE + i))
            {
                mapbuffer.top().updateAtPos(select.first, select.second, i+1, false, true);
            }
        }

        // ----------
        // DEBUG
        // ----------
    }

    // UnloadTexture() and CloseWindow() are called automatically.

    return 0;
}