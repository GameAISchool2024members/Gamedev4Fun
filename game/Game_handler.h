#include <chrono>
#include <fstream>
#include "General.h"

#ifndef SS_GAME_GAME_HANDLER_H
#define SS_GAME_GAME_HANDLER_H

class Game_handler {
    General_handler handler{};
    mt19937_64 rng;
    int average_num_of_enemies = 4;
    double karateka_probability = .5;
    ofstream file;

    uniform_real_distribution<double> uniform_unit = uniform_real_distribution<double>();
public:
    Game_handler() {
        rng = mt19937_64(duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
    }

    void game();

    vector<int> build_floor();

    vector<vector<bool>> build_room(const vector<Direction> &directions);

    vector<Enemy> build_enemies(const vector<vector<bool>> &room);

    void adapt(int prev_room_hp);
};

vector<int> find_neighbors(int r);

#endif //SS_GAME_GAME_HANDLER_H
