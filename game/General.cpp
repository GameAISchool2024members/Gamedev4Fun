#include <bits/stdc++.h>
#include "General.h"

void General_handler::initialize(bool final_room) {
    rng = mt19937_64(duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    Mix_Init(0);
    Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 1024);
    TTF_Init();
    SDL_CreateWindowAndRenderer(800, 800, 0, &window, &renderer);
    sprite_map[WALL].push_back(IMG_LoadTexture(renderer, R"(.\sprites\Brick_wall.jpg)"));
    sprite_map[EMPTY_BOX].push_back(IMG_LoadTexture(renderer, R"(.\sprites\Yellow_texture.jpg)"));
    sprite_map[CLOWN].push_back(IMG_LoadTexture(renderer, R"(.\sprites\Idle clown.png)"));
    sprite_map[CLOWN].push_back(IMG_LoadTexture(renderer, R"(.\sprites\Throw.png)"));
    sprite_map[KARATEKA].push_back(IMG_LoadTexture(renderer, R"(.\sprites\Front.png)"));
    sprite_map[KARATEKA].push_back(IMG_LoadTexture(renderer, R"(.\sprites\Walk front 1.png)"));
    sprite_map[KARATEKA].push_back(IMG_LoadTexture(renderer, R"(.\sprites\Walk front 2.png)"));
    sprite_map[KARATEKA].push_back(IMG_LoadTexture(renderer, R"(.\sprites\Mega kick right.png)"));
    sprite_map[KARATEKA].push_back(IMG_LoadTexture(renderer, R"(.\sprites\Mega kick left.png)"));
    sprite_map[PROTAGONIST].push_back(IMG_LoadTexture(renderer, R"(.\sprites\Smile.png)"));
    sprite_map[PROTAGONIST_SHOT].push_back(IMG_LoadTexture(renderer, R"(.\sprites\Orange_ball.png)"));
    sprite_map[ENEMY_SHOT].push_back(IMG_LoadTexture(renderer, R"(.\sprites\Blue_ball.png)"));
    swing_texture = IMG_LoadTexture(renderer, R"(.\sprites\Swing.png)");
    trapdoor_texture = IMG_LoadTexture(renderer, R"(.\sprites\Trapdoor.png)");
    explosion_texture = IMG_LoadTexture(renderer, R"(.\sprites\Explosion.png)");
    karateka_kick_sound = Mix_LoadWAV(R"(.\sounds\Karateka_kick_sound.wav)");
    clown_charge_sound = Mix_LoadWAV(R"(.\sounds\Clown_charge_sound.wav)");
    clown_explosion_sound = Mix_LoadWAV(R"(.\sounds\Clown_explosion_sound.wav)");
    background_music = Mix_LoadMUS(R"(.\sounds\Background_music.wav)");
    victory_sound = Mix_LoadWAV(R"(.\sounds\Applause.wav)");
    game_over_sound = Mix_LoadWAV(R"(.\sounds\Game_over.wav)");
    isaac_font = TTF_OpenFont(R"(.\fonts\Isaac_font.ttf)", 200);
    standard_font = TTF_OpenFont(R"(.\fonts\ColabBol.otf)", 200);
    protagonist = Entity(vector<int>{400, 400}, 25, 30, 0, PROTAGONIST);
    enemies = {};
    game_stats = Game_stats(4, 1.0 / 60);
    floor_data = Floor_data();
    adaptations = {};
    this->final_room = final_room;
    game_time = 0;
    room_time = 0;
}

void General_handler::room_change_animation(const vector<vector<bool>> &new_room, const vector<Enemy> &new_room_enemies, Direction direction) {
    room_time = 0;
    constexpr double alpha_tick = 1.5;
    constexpr double slide_tick = 10;
    if (room.empty()) {
        room = new_room;
        enemies = new_room_enemies;
        SDL_SetTextureBlendMode(sprite_map[WALL][0], SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(sprite_map[EMPTY_BOX][0], SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(sprite_map[PROTAGONIST][0], SDL_BLENDMODE_BLEND);
        for (const Entity &e : enemies) {
            SDL_SetTextureBlendMode(sprite_map[e.type][0], SDL_BLENDMODE_BLEND);
        }
        for (double i = 0; i <= 255; i += alpha_tick) {
            SDL_SetTextureAlphaMod(sprite_map[WALL][0], i);
            SDL_SetTextureAlphaMod(sprite_map[EMPTY_BOX][0], i);
            SDL_SetTextureAlphaMod(sprite_map[PROTAGONIST][0], i);
            for (const Entity &e : enemies) {
                SDL_SetTextureAlphaMod(sprite_map[e.type][0], i);
            }
            SDL_SetTextureAlphaMod(explosion_texture, i);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            base_render();
        }
        SDL_SetTextureAlphaMod(sprite_map[WALL][0], 255);
        SDL_SetTextureAlphaMod(sprite_map[EMPTY_BOX][0], 255);
        SDL_SetTextureAlphaMod(sprite_map[PROTAGONIST][0], 255);
        for (const Entity &e : enemies) {
            SDL_SetTextureAlphaMod(sprite_map[e.type][0], 255);
        }
        SDL_SetTextureAlphaMod(explosion_texture, 255);
        return;
    }
    if (new_room.empty()) {
        SDL_SetTextureBlendMode(sprite_map[WALL][0], SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(sprite_map[EMPTY_BOX][0], SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(sprite_map[PROTAGONIST][0], SDL_BLENDMODE_BLEND);
        for (const Entity &e : enemies) {
            SDL_SetTextureBlendMode(sprite_map[e.type][e.current_sprite], SDL_BLENDMODE_BLEND);
        }
        for (double i = 255; i >= 0; i -= alpha_tick) {
            SDL_SetTextureAlphaMod(sprite_map[WALL][0], i);
            SDL_SetTextureAlphaMod(sprite_map[EMPTY_BOX][0], i);
            SDL_SetTextureAlphaMod(sprite_map[PROTAGONIST][0], i);
            for (const Entity &e : enemies) {
                SDL_SetTextureAlphaMod(sprite_map[e.type][e.current_sprite], i);
            }
            for (const Entity &e : enemy_shots) {
                SDL_SetTextureAlphaMod(sprite_map[e.type][0], i);
            }
            for (const Entity &e : protagonist_shots) {
                SDL_SetTextureAlphaMod(sprite_map[e.type][0], i);
            }
            SDL_SetTextureAlphaMod(explosion_texture, i);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            base_render();
        }
        SDL_SetTextureAlphaMod(sprite_map[WALL][0], 255);
        SDL_SetTextureAlphaMod(sprite_map[EMPTY_BOX][0], 255);
        SDL_SetTextureAlphaMod(sprite_map[PROTAGONIST][0], 255);
        for (const Entity &e : enemies) {
            SDL_SetTextureAlphaMod(sprite_map[e.type][0], 255);
        }
        for (const Entity &e : enemy_shots) {
            SDL_SetTextureAlphaMod(sprite_map[e.type][0], 255);
        }
        for (const Entity &e : protagonist_shots) {
            SDL_SetTextureAlphaMod(sprite_map[e.type][0], 255);
        }
        SDL_SetTextureAlphaMod(explosion_texture, 255);
        room = new_room;
        enemies = new_room_enemies;
        return;
    }
    for (double i = 0; i < 800; i += slide_tick) {
        shifted_render(new_room, i, direction);
    }
    room = new_room;
    enemies = new_room_enemies;
}

void General_handler::base_render() noexcept {
    SDL_RenderClear(renderer);
    static auto map_crop = SDL_Rect(0, 0, 50, 50);
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            map_crop.x = 50 * i;
            map_crop.y = 50 * j;
            SDL_RenderCopy(renderer, sprite_map[room[i][j] ? WALL : EMPTY_BOX][0], nullptr, &map_crop);
        }
    }
    SDL_SetTextureColorMod(sprite_map[PROTAGONIST][protagonist.current_sprite], 255, 255 * (1 - protagonist.hit_tick / 60.0), 255 * (1 - protagonist.hit_tick / 60.0));
    protagonist.render(renderer, sprite_map[PROTAGONIST][protagonist.current_sprite]);
    if (protagonist_swing) {
        SDL_SetTextureAlphaMod(swing_texture, 255 * 30 / protagonist_swing);
        double angle = atan2(protagonist_swing_direction[1], protagonist_swing_direction[0]) * 180 / M_PI + 45;
        const auto rect = SDL_Rect(protagonist.position[0] - 3 * protagonist.radius, protagonist.position[1] - 3 * protagonist.radius, 6 * protagonist.radius, 6 * protagonist.radius);
        SDL_RenderCopyEx(renderer, swing_texture, nullptr, &rect, angle, nullptr, SDL_FLIP_NONE);
        --protagonist_swing;
    }
    for (const Enemy &e: enemies) {
        if (e.type == KARATEKA || (clown_explosion[e.id] == 0 && clown_charge[e.id] == 0)) {
            SDL_SetTextureColorMod(sprite_map[e.type][e.current_sprite], 255, 255 * (1 - e.hit_tick / 60.0), 255 * (1 - e.hit_tick / 60.0));
            e.render(renderer, sprite_map[e.type][e.current_sprite]);
        } else {
            if (clown_explosion[e.id] != 0) {
                auto rect = SDL_Rect(e.position[0] - 20 + 2 * gauss(rng), e.position[1] - 20 + 2 * gauss(rng), 80, 80);
                SDL_RenderCopy(renderer, explosion_texture, nullptr, &rect);
            } else {
                SDL_SetTextureColorMod(sprite_map[CLOWN][0], 0, 255, 255);
                e.render(renderer, sprite_map[e.type][0]);
            }
        }
    }
    for (const Entity &e: protagonist_shots) {
        e.render(renderer, sprite_map[e.type][e.current_sprite]);
    }
    for (const Entity &e: enemy_shots) {
        e.render(renderer, sprite_map[e.type][e.current_sprite]);
    }
    if (final_room && enemies.empty()) {
        static const SDL_Rect trapdoor_rect(375, 375, 50, 50);
        SDL_RenderCopy(renderer, trapdoor_texture, nullptr, &trapdoor_rect);
    }
    const SDL_Rect hp_bar = SDL_Rect(10, 10, max(0, 10 * protagonist.hp), 20);
    int color[2];
    if (protagonist.hp < 15) {
        color[0] = 255;
        color[1] = 17 * protagonist.hp;
    } else {
        color[0] = 255 - 17 * (protagonist.hp - 15);
        color[1] = 255;
    }
    SDL_SetRenderDrawColor(renderer, color[0], color[1], 0, 255);
    SDL_RenderFillRect(renderer, &hp_bar);
    SDL_Delay(max(100.0 / 6.0 - static_cast<double>(SDL_GetTicks() - framerate_last_tick), 0.0));
    SDL_RenderPresent(renderer);
    framerate_last_tick = SDL_GetTicks();
}

void General_handler::shifted_render(const vector<vector<bool>> &new_room, const int shift, const Direction direction) noexcept {
    SDL_RenderClear(renderer);
    static auto map_crop = SDL_Rect(0, 0, 50, 50);
    static int new_map_crop[2];
    static int tick[2];
    switch (direction) {
        case UP:
            tick[0] = 0;
            tick[1] = shift;
            new_map_crop[0] = 0;
            new_map_crop[1] = -800;
            break;
        case DOWN:
            tick[0] = 0;
            tick[1] = -shift;
            new_map_crop[0] = 0;
            new_map_crop[1] = 800;
            break;
        case RIGHT:
            tick[0] = -shift;
            tick[1] = 0;
            new_map_crop[0] = 800;
            new_map_crop[1] = 0;
            break;
        case LEFT:
            tick[0] = shift;
            tick[1] = 0;
            new_map_crop[0] = -800;
            new_map_crop[1] = 0;
            break;
    }
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            map_crop.x = 50 * i + tick[0];
            map_crop.y = 50 * j + tick[1];
            SDL_RenderCopy(renderer, sprite_map[room[i][j] ? WALL : EMPTY_BOX][0], nullptr, &map_crop);
            map_crop.x = 50 * i + new_map_crop[0] + tick[0];
            map_crop.y = 50 * j + new_map_crop[1] + tick[1];
            SDL_RenderCopy(renderer, sprite_map[new_room[i][j] ? WALL : EMPTY_BOX][0], nullptr, &map_crop);
        }
    }
    Entity protagonist_copy = Entity(protagonist);
    protagonist_copy.position[0] += new_map_crop[0] + tick[0];
    protagonist_copy.position[1] += new_map_crop[1] + tick[1];
    protagonist_copy.render(renderer, sprite_map[PROTAGONIST][0]);
    SDL_Delay(max(100.0 / 6.0 - static_cast<double>(SDL_GetTicks() - framerate_last_tick), 0.0));
    SDL_RenderPresent(renderer);
    framerate_last_tick = SDL_GetTicks();
}

bool General_handler::poll_events_and_update_positions() noexcept {
    ++room_time;
    ++game_time;
    static SDL_Event event;
    Direction shoot = NONE;
    bool swing = false;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return false;
        }
        if (event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
                case SDLK_p:
                    pause = !pause;
                    break;
                case SDLK_k:
                    // instakill button for testing
                    //enemies.clear();
                    break;
                case SDLK_j:
                    // instadeath button for testing
                    //protagonist.hp = 0;
                case SDLK_q:
                    if (pause) {
                        return false;
                    }
            }
        }
    }
    if (pause) {
        return true;
    }
    const Uint8 *keyboard_status = SDL_GetKeyboardState(nullptr);
    if (keyboard_status[SDL_SCANCODE_W]) {
        protagonist.velocity[1] = -8;
    }
    if (keyboard_status[SDL_SCANCODE_S]) {
        protagonist.velocity[1] = 8;
    }
    if (keyboard_status[SDL_SCANCODE_D]) {
        protagonist.velocity[0] = 8;
    }
    if (keyboard_status[SDL_SCANCODE_A]) {
        protagonist.velocity[0] = -8;
    }
    if (keyboard_status[SDL_SCANCODE_LEFT]) {
        shoot = LEFT;
    }
    if (keyboard_status[SDL_SCANCODE_RIGHT]) {
        shoot = RIGHT;
    }
    if (keyboard_status[SDL_SCANCODE_UP]) {
        shoot = UP;
    }
    if (keyboard_status[SDL_SCANCODE_DOWN]) {
        shoot = DOWN;
    }
    if (keyboard_status[SDL_SCANCODE_SPACE]) {
        swing = true;
    }

    //protagonist attacks
    if (protagonist.action_tick) {
        --protagonist.action_tick;
    } else if (swing) {
        if (!enemies.empty()) {
            ++floor_data.protagonist_swings;
        }
        protagonist.action_tick = 30;
        protagonist_swing = 6;
        protagonist_swing_direction = vector<int>(2);
        switch (shoot) {
            case UP:
                protagonist_swing_direction[0] = 0;
                protagonist_swing_direction[1] = -1;
                break;
            case DOWN:
                protagonist_swing_direction[0] = 0;
                protagonist_swing_direction[1] = 1;
                break;
            case RIGHT:
                protagonist_swing_direction[0] = 1;
                protagonist_swing_direction[1] = 0;
                break;
            case LEFT:
                protagonist_swing_direction[0] = -1;
                protagonist_swing_direction[1] = 0;
                break;
            case NONE:
                protagonist.action_tick = 0;
                protagonist_swing = 0;
        }
        bool one_hit = false;
        for (Enemy &e : enemies) {
            double distance_norm = point_point_distance(protagonist.position, e.position);
            if (distance_norm < 5 * protagonist.radius + e.radius) {
                vector<int> distance{e.position[0] - protagonist.position[0], e.position[1] - protagonist.position[1]};
                if (distance[0] * protagonist_swing_direction[0] + distance[1] * protagonist_swing_direction[1] > distance_norm * sqrt(.5)) {
                    if (!one_hit) {
                        ++floor_data.protagonist_swings_hit;
                        one_hit = true;
                    }
                    e.hit_tick = 60;
                    e.hp -= 8;
                }
            }
        }
    } else {
        protagonist.action_tick = 20;
        if (shoot != NONE) {
            if (!enemies.empty()) {
                ++floor_data.protagonist_shots_fired;
            }
            protagonist_shots.emplace_back(protagonist.position, 10, 0, ++id_counter, PROTAGONIST_SHOT);
        }
        switch (shoot) {
            case UP:
                protagonist_shots.rbegin()->velocity[0] = 0;
                protagonist_shots.rbegin()->velocity[1] = -20;
                break;
            case DOWN:
                protagonist_shots.rbegin()->velocity[0] = 0;
                protagonist_shots.rbegin()->velocity[1] = 20;
                break;
            case RIGHT:
                protagonist_shots.rbegin()->velocity[0] = 20;
                protagonist_shots.rbegin()->velocity[1] = 0;
                break;
            case LEFT:
                protagonist_shots.rbegin()->velocity[0] = -20;
                protagonist_shots.rbegin()->velocity[1] = 0;
                break;
            case NONE:
                protagonist.action_tick = 0;
        }
    }

    //protagonist movement
    map<int, vector<int>> previous_positions{{protagonist.id, vector<int>{protagonist.position[0], protagonist.position[1]}}};
    avoid_wall_collision(protagonist);
    protagonist.position[0] += protagonist.velocity[0];
    protagonist.position[1] += protagonist.velocity[1];

    //enemy movement
    for (Enemy &e: enemies) {
        if (e.hit_tick != 0) {
            --e.hit_tick;
        }
        previous_positions[e.id] = vector<int>(e.position);
        vector<double> protagonist_direction{static_cast<double>(protagonist.position[0] - e.position[0]), static_cast<double>(protagonist.position[1] - e.position[1])};
        double norm = sqrt(protagonist_direction[0] * protagonist_direction[0] + protagonist_direction[1] * protagonist_direction[1]);
        if (norm != 0) {
            protagonist_direction[0] /= norm;
            protagonist_direction[1] /= norm;
        } else {
            protagonist_direction[0] = protagonist_direction[1] = 0;
        }
        switch (e.type) {
            case KARATEKA:
                if (karateka_kick_animation[e.id] == 0) {
                    if (uniform_real(rng) < 1.0 / 120) {
                        karateka_kick_animation[e.id] = 1;
                        Mix_PlayChannel(-1, karateka_kick_sound, 0);
                        e.current_sprite = protagonist_direction[0] > 0 ? 3 : 4;
                    } else {
                        e.velocity[0] = static_cast<int>(round(protagonist_direction[0] * e.movement_average + e.movement_control * gauss(rng)));
                        e.velocity[1] = static_cast<int>(round(protagonist_direction[1] * e.movement_average + e.movement_control * gauss(rng)));
                        if (sprite_clock[e.id] > 20) {
                            if (e.current_sprite < 2) {
                                e.current_sprite = 2;
                            } else {
                                e.current_sprite = 1;
                            }
                            sprite_clock[e.id] = 0;
                        } else {
                            ++sprite_clock[e.id];
                        }
                    }
                    avoid_wall_collision(e);
                    e.position[0] += e.velocity[0];
                    e.position[1] += e.velocity[1];
                } else {
                    if (++karateka_kick_animation[e.id] > 30) {
                        if (avoid_wall_collision(e)) {
                            karateka_kick_animation[e.id] = 0;
                            sprite_clock[e.id] = 0;
                            e.current_sprite = 1;
                        }
                        e.position[0] += e.velocity[0];
                        e.position[1] += e.velocity[1];
                    } else if (karateka_kick_animation[e.id] < 20) {
                        e.velocity[0] = static_cast<int>(round(protagonist_direction[0] * 20));
                        e.velocity[1] = static_cast<int>(round(protagonist_direction[1] * 20));
                        e.current_sprite = protagonist_direction[0] > 0 ? 3 : 4;
                    }
                }
                break;
            case CLOWN:
                if (clown_explosion[e.id] == 1) {
                    e.hp = 0;
                } else if (clown_explosion[e.id] == 0) {
                    if (clown_charge[e.id] != 0) {
                        --clown_charge[e.id];
                        e.velocity[0] = static_cast<int>(round(6 * protagonist_direction[0]));
                        e.velocity[1] = static_cast<int>(round(6 * protagonist_direction[1]));
                        avoid_wall_collision(e);
                        e.position[0] += e.velocity[0];
                        e.position[1] += e.velocity[1];
                        if (collide(previous_positions, e, protagonist)) {
                            clown_explosion[e.id] = 45;
                            Mix_PlayChannel(-1, clown_explosion_sound, 0);
                            protagonist.hp -= 8;
                            protagonist.hit_tick = 60;
                            ++floor_data.enemy_explosion_hits;
                        } else if (clown_charge[e.id] == 1) {
                            clown_explosion[e.id] = 45;
                            Mix_PlayChannel(-1, clown_explosion_sound, 0);
                        }
                    } else {
                        if (sprite_clock[e.id] != 0) {
                            --sprite_clock[e.id];
                            if (sprite_clock[e.id] == 0) {
                                e.current_sprite = 0;
                            }
                        }
                        if (uniform_real(rng) < game_stats.clown_shoot_probability) {
                            ++floor_data.enemy_shots_fired;
                            enemy_shots.emplace_back(e.position, 10, 0, ++id_counter, ENEMY_SHOT);
                            enemy_shots.rbegin()->velocity[0] = static_cast<int>(round(protagonist_direction[0] * 15 + 2 * gauss(rng)));
                            enemy_shots.rbegin()->velocity[1] = static_cast<int>(round(protagonist_direction[1] * 15 + 2 * gauss(rng)));
                            sprite_clock[e.id] = 20;
                            e.current_sprite = 1;
                        } else if (room_time > 150 && uniform_real(rng) < game_stats.clown_shoot_probability * .1) {
                            clown_charge[e.id] = 120;
                            Mix_PlayChannel(-1, clown_charge_sound, 0);
                            e.velocity[0] = static_cast<int>(round(6 * protagonist_direction[0]));
                            e.velocity[0] = static_cast<int>(round(6 * protagonist_direction[1]));
                        } else if (uniform_real(rng) < e.movement_control) {
                            e.velocity[0] = static_cast<int>(round(gauss(rng))) + (2 * coin_flip(rng) - 1) * e.movement_average;
                            e.velocity[1] = static_cast<int>(round(gauss(rng))) + (2 * coin_flip(rng) - 1) * e.movement_average;
                        }
                        avoid_wall_collision(e);
                        e.position[0] += e.velocity[0];
                        e.position[1] += e.velocity[1];
                    }
                } else {
                    --clown_explosion[e.id];
                }
                break;
            default:
                break;
        }
    }

    // enemy shots wall collisions
    for (int i = 0; i < enemy_shots.size(); ++i) {
        previous_positions[enemy_shots[i].id] = vector<int>(enemy_shots[i].position);
        if (avoid_wall_collision(enemy_shots[i])) {
            enemy_shots.erase(next(enemy_shots.begin(), i));
            --i;
        } else {
            enemy_shots[i].position[0] += enemy_shots[i].velocity[0];
            enemy_shots[i].position[1] += enemy_shots[i].velocity[1];
        }
    }

    // protagonist shots wall collisions
    for (int i = 0; i < protagonist_shots.size(); ++i) {
        previous_positions[protagonist_shots[i].id] = vector<int>(protagonist_shots[i].position);
        if (avoid_wall_collision(protagonist_shots[i])) {
            protagonist_shots.erase(next(protagonist_shots.begin(), i));
            --i;
        } else {
            protagonist_shots[i].position[0] += protagonist_shots[i].velocity[0];
            protagonist_shots[i].position[1] += protagonist_shots[i].velocity[1];
        }
    }

    // remove dead enemies
    for (int i = 0; i < enemies.size(); ++i) {
        if (enemies[i].hp <= 0) {
            enemies.erase(next(enemies.begin(), i));
            --i;
        }
    }

    // hits on protagonist
    if (protagonist.hit_tick) {
        --protagonist.hit_tick;
    } else {
        for (const Enemy &e: enemies) {
            if (collide(previous_positions, protagonist, e)) {
                ++floor_data.enemy_contact_hits;
                protagonist.hit_tick = 60;
                protagonist.hp -= 3;
                break;
            }
        }
        if (!protagonist.hit_tick) {
            for (int i = 0; i < enemy_shots.size(); ++i) {
                if (collide(previous_positions, enemy_shots[i], protagonist)) {
                    ++floor_data.enemy_shots_hit;
                    enemy_shots.erase(next(enemy_shots.begin(), i));
                    protagonist.hit_tick = 60;
                    protagonist.hp -= 2;
                    --i;
                }
            }
        }
    }

    // protagonist shots hit
    for (Enemy &e: enemies) {
        for (int i = 0; i < protagonist_shots.size(); ++i) {
            if (collide(previous_positions, protagonist_shots[i], e)) {
                ++floor_data.protagonist_shots_hit;
                protagonist_shots.erase(next(protagonist_shots.begin(), i));
                e.hp -= 5;
                e.hit_tick = 30;
                break;
            }
        }
    }
    protagonist.velocity[0] = protagonist.velocity[1] = 0;
    return protagonist.hp > 0;
}

bool General_handler::collide(const map<int, vector<int>> &previous_positions, const Entity &entity1, const Entity &entity2) {
    vector<double> distances(4);
    distances[0] = point_point_distance(entity1.position, entity2.position);
    distances[1] = point_point_distance(entity1.position, previous_positions.at(entity2.id));
    distances[2] = point_point_distance(previous_positions.at(entity1.id), entity2.position);
    distances[3] = point_point_distance(previous_positions.at(entity1.id), previous_positions.at(entity2.id));
    return (*min_element(distances.begin(), distances.end())) < (entity1.radius + entity2.radius);
}

bool General_handler::avoid_wall_collision(Entity &entity) {
    bool collision = false;
    vector<int> position_cell{entity.position[0] / 50, entity.position[1] / 50};
    if (position_cell[0] < 0 || position_cell[0] > 15 || position_cell[1] < 0 || position_cell[1] > 15) {
        entity.velocity[0] = 0;
        entity.velocity[1] = 0;
        return true;
    }
    if (entity.position[0] % 50 <= entity.radius &&
        ((position_cell[0] > 0 && room[position_cell[0] - 1][position_cell[1]]) ||
         (position_cell[0] == 0))) {
        if (entity.velocity[0] < 0) {
            collision = true;
            entity.velocity[0] = 0;
        }
    }
    if (entity.position[0] % 50 >= 50 - entity.radius &&
        ((position_cell[0] < 15 && room[position_cell[0] + 1][position_cell[1]]) ||
         (position_cell[0] == 15))) {
        if (entity.velocity[0] > 0) {
            collision = true;
            entity.velocity[0] = 0;
        }
    }
    if (entity.position[1] % 50 <= entity.radius &&
        ((position_cell[1] > 0 && room[position_cell[0]][position_cell[1] - 1]) ||
         (position_cell[1] == 0))) {
        if (entity.velocity[1] < 0) {
            collision = true;
            entity.velocity[1] = 0;
        }
    }
    if (entity.position[1] % 50 >= 50 - entity.radius &&
        ((position_cell[1] < 15 && room[position_cell[0]][position_cell[1] + 1]) ||
         (position_cell[1] == 15))) {
        if (entity.velocity[1] > 0) {
            collision = true;
            entity.velocity[1] = 0;
        }
    }
    return collision;
}

void General_handler::initial_timer() noexcept {
    static const SDL_Rect text_box = SDL_Rect(200, 200, 400, 400);
    for (int i = 3; i > 0; --i) {
        SDL_Surface *text_surface = TTF_RenderText_Solid(standard_font, to_string(i).c_str(), SDL_Color{0, 0, 0});
        SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        SDL_FreeSurface(text_surface);
        for (int j = 0; j < 60; ++j) {
            SDL_RenderClear(renderer);
            static auto map_crop = SDL_Rect(0, 0, 50, 50);
            for (int x = 0; x < 16; ++x) {
                for (int y = 0; y < 16; ++y) {
                    map_crop.x = 50 * x;
                    map_crop.y = 50 * y;
                    SDL_RenderCopy(renderer, sprite_map[room[x][y] ? WALL : EMPTY_BOX][0], nullptr, &map_crop);
                }
            }
            protagonist.render(renderer, sprite_map[PROTAGONIST][0]);
            for (const Enemy &e : enemies) {
                e.render(renderer, sprite_map[e.type][0]);
            }
            SDL_RenderCopy(renderer, text_texture, nullptr, &text_box);
            SDL_Delay(max(100.0 / 6.0 - static_cast<double>(SDL_GetTicks() - framerate_last_tick), 0.0));
            SDL_RenderPresent(renderer);
            framerate_last_tick = SDL_GetTicks();
        }
        SDL_DestroyTexture(text_texture);
    }
}

void General_handler::victory_screen() noexcept {
    Mix_HaltMusic();
    Mix_PlayChannel(-1, victory_sound, 0);
    SDL_Surface *text_surface = TTF_RenderText_Solid(isaac_font, "YOU WIN!", SDL_Color{255, 255, 255});
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_SetTextureBlendMode(text_texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    const SDL_Rect text_box = SDL_Rect(50, 300, 700, 200);
    SDL_Event event;
    for (int i = 0; i <= 255; i += 1) {
        SDL_RenderClear(renderer);
        SDL_SetTextureAlphaMod(text_texture, i);
        SDL_RenderCopy(renderer, text_texture, nullptr, &text_box);
        SDL_Delay(max(100.0 / 6.0 - static_cast<double>(SDL_GetTicks() - framerate_last_tick), 0.0));
        SDL_RenderPresent(renderer);
        framerate_last_tick = SDL_GetTicks();
        while(SDL_PollEvent(&event)) {}
    }
    for (int i = 255; i >= 0; i -= 1) {
        SDL_RenderClear(renderer);
        SDL_SetTextureAlphaMod(text_texture, i);
        SDL_RenderCopy(renderer, text_texture, nullptr, &text_box);
        SDL_Delay(max(100.0 / 6.0 - static_cast<double>(SDL_GetTicks() - framerate_last_tick), 0.0));
        SDL_RenderPresent(renderer);
        framerate_last_tick = SDL_GetTicks();
        while(SDL_PollEvent(&event)) {}
    }
    SDL_FreeSurface(text_surface);
    stats_screen();
}

void General_handler::defeat_screen() noexcept {
    Mix_HaltMusic();
    Mix_PlayChannel(-1, game_over_sound, 0);
    SDL_Surface *text_surface = TTF_RenderText_Solid(isaac_font, "GAME OVER", SDL_Color{255, 255, 255});
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_SetTextureBlendMode(text_texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    const SDL_Rect text_box = SDL_Rect(50, 300, 700, 200);
    SDL_Event event;
    for (int i = 0; i <= 255; i += 1) {
        SDL_RenderClear(renderer);
        SDL_SetTextureAlphaMod(text_texture, i);
        SDL_RenderCopy(renderer, text_texture, nullptr, &text_box);
        SDL_Delay(max(100.0 / 6.0 - static_cast<double>(SDL_GetTicks() - framerate_last_tick), 0.0));
        SDL_RenderPresent(renderer);
        framerate_last_tick = SDL_GetTicks();
        while(SDL_PollEvent(&event)) {}
    }
    for (int i = 255; i >= 0; i -= 1) {
        SDL_RenderClear(renderer);
        SDL_SetTextureAlphaMod(text_texture, i);
        SDL_RenderCopy(renderer, text_texture, nullptr, &text_box);
        SDL_Delay(max(100.0 / 6.0 - static_cast<double>(SDL_GetTicks() - framerate_last_tick), 0.0));
        SDL_RenderPresent(renderer);
        framerate_last_tick = SDL_GetTicks();
        while(SDL_PollEvent(&event)) {}
    }
    SDL_FreeSurface(text_surface);
    stats_screen();
}

void General_handler::stats_screen() noexcept {
    const double total_hits = max(floor_data.enemy_shots_hit + floor_data.enemy_explosion_hits + floor_data.enemy_contact_hits, 1);
    vector<string> stats = {
            "ENEMIES KILLED: " + to_string(total_n_of_enemies - enemies.size()),
            "FINAL HP: " + to_string(max(protagonist.hp, 0)),
            "TOTAL TIME: " + to_string(game_time / 60) + "s",
            "SHOT ACCURACY: " + to_string(static_cast<int>(round(100 * floor_data.protagonist_shots_hit / static_cast<double>(max(floor_data.protagonist_shots_fired, 1))))) + "%",
            "ENEMY SHOT ACCURACY: " + to_string(static_cast<int>(round(100 * floor_data.enemy_shots_hit / static_cast<double>(max(floor_data.enemy_shots_fired, 1))))) + "%",
            "TOTAL TIMES HIT: " + to_string(floor_data.enemy_shots_hit + floor_data.enemy_explosion_hits + floor_data.enemy_contact_hits),
            to_string(static_cast<int>(round(100 * floor_data.enemy_contact_hits / total_hits))) + "% CONTACT; " +
            to_string(static_cast<int>(round(100 * floor_data.enemy_shots_hit / total_hits))) + "% SHOT; " +
            to_string(static_cast<int>(round(100 * floor_data.enemy_explosion_hits / total_hits))) + "% EXPLOSION"
    };
    int n_of_lines = stats.size();
    double max_size = max_element(stats.begin(), stats.end(), [](const string &s1, const string &s2) { return s1.size() < s2.size(); })->size();
    double text_height = 600.0 / n_of_lines;
    SDL_Surface *surface = TTF_RenderUTF8_Solid(isaac_font, "YOUR STATS", SDL_Color{255, 255, 255});
    SDL_Texture *title_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    vector<SDL_Texture *> text_textures{};
    for (const string &s: stats) {
        surface = TTF_RenderUTF8_Solid(standard_font, s.c_str(), SDL_Color{255, 255, 255});
        text_textures.push_back(SDL_CreateTextureFromSurface(renderer, surface));
        SDL_FreeSurface(surface);
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    static SDL_Rect text_box = SDL_Rect();
    static const SDL_Rect title_box = SDL_Rect(10, 10, 780, 100);
    text_box.h = text_height;
    for (int i = 0; i <= 255; i += 2) {
        SDL_RenderClear(renderer);
        SDL_SetTextureAlphaMod(title_texture, i);
        SDL_RenderCopy(renderer, title_texture, nullptr, &title_box);
        for (int j = 0; j < n_of_lines; ++j) {
            SDL_SetTextureAlphaMod(text_textures[j], i);
            text_box.w = static_cast<int>(780.0 * stats[j].size() / max_size);
            text_box.x = 400 - text_box.w / 2;
            text_box.y = 160 + j * text_height;
            SDL_RenderCopy(renderer, text_textures[j], nullptr, &text_box);
        }
        SDL_Delay(max(100.0 / 6.0 - static_cast<double>(SDL_GetTicks() - framerate_last_tick), 0.0));
        SDL_RenderPresent(renderer);
        framerate_last_tick = SDL_GetTicks();
    }
    bool keep_open = true;
    bool switch_stats = false;
    while (keep_open && !switch_stats) {
        static SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    keep_open = false;
                    break;
                case SDL_KEYUP:
                    switch_stats = true;
            }
        }
    }
    if (switch_stats) {
        for (int i = 255; i >= 0; i -= 2) {
            SDL_RenderClear(renderer);
            SDL_SetTextureAlphaMod(title_texture, i);
            SDL_RenderCopy(renderer, title_texture, nullptr, &title_box);
            for (int j = 0; j < n_of_lines; ++j) {
                SDL_SetTextureAlphaMod(text_textures[j], i);
                text_box.w = static_cast<int>(780.0 * stats[j].size() / max_size);
                text_box.x = 400 - text_box.w / 2;
                text_box.y = 160 + j * text_height;
                SDL_RenderCopy(renderer, text_textures[j], nullptr, &text_box);
            }
            SDL_Delay(max(100.0 / 6.0 - static_cast<double>(SDL_GetTicks() - framerate_last_tick), 0.0));
            SDL_RenderPresent(renderer);
            framerate_last_tick = SDL_GetTicks();
        }
        SDL_DestroyTexture(title_texture);
        for (SDL_Texture *texture: text_textures) {
            SDL_DestroyTexture(texture);
        }
        text_textures.clear();
        stats.clear();
        stats = {
                "CLOWN SHOOT RATE (shots/s):", "",
                "KARATEKA SPEED:", "",
                "KARATEKA SPAWN PERCENTAGE:", "",
                "AVERAGE NUMBER OF ENEMIES:", ""
        };
        stringstream csp_stream, ks_stream, ksr_stream, ane_stream;
        csp_stream << fixed << setprecision(2) << 1.0;
        ks_stream << 4;
        ksr_stream << "50%";
        ane_stream << 4;
        for (const Room_adaptations &ra: adaptations) {
            csp_stream << "->" << fixed << setprecision(2) << 60.0 * ra.clown_shoot_probability;
            ks_stream << "->" << ra.karateka_average_speed;
            ksr_stream << "->" << static_cast<int>(round(100 * ra.karateka_probability)) << "%";
            ane_stream << "->" << ra.average_number_of_enemies;
        }
        stats[1] = csp_stream.str();
        stats[3] = ks_stream.str();
        stats[5] = ksr_stream.str();
        stats[7] = ane_stream.str();
        n_of_lines = stats.size();
        max_size = max_element(stats.begin(), stats.end(), [](const string &s1, const string &s2) { return s1.size() < s2.size(); })->size();
        text_height = 600.0 / n_of_lines;
        surface = TTF_RenderUTF8_Solid(isaac_font, "ADAPTATIONS", SDL_Color{255, 255, 255});
        title_texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        for (const string &s: stats) {
            surface = TTF_RenderUTF8_Solid(standard_font, s.c_str(), SDL_Color{255, 255, 255});
            text_textures.push_back(SDL_CreateTextureFromSurface(renderer, surface));
            SDL_FreeSurface(surface);
        }
        text_box.h = text_height;
        for (int i = 0; i <= 255; i += 2) {
            SDL_RenderClear(renderer);
            SDL_SetTextureAlphaMod(title_texture, i);
            SDL_RenderCopy(renderer, title_texture, nullptr, &title_box);
            for (int j = 0; j < n_of_lines; ++j) {
                SDL_SetTextureAlphaMod(text_textures[j], i);
                text_box.w = static_cast<int>(780.0 * stats[j].size() / max_size);
                text_box.x = 400 - text_box.w / 2;
                text_box.y = 140 + j * text_height + 20 * (j >> 1);
                SDL_RenderCopy(renderer, text_textures[j], nullptr, &text_box);
            }
            SDL_Delay(max(100.0 / 6.0 - static_cast<double>(SDL_GetTicks() - framerate_last_tick), 0.0));
            SDL_RenderPresent(renderer);
            framerate_last_tick = SDL_GetTicks();
        }
        keep_open = true;
        switch_stats = false;
        while (keep_open && !switch_stats) {
            static SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        keep_open = false;
                        break;
                    case SDL_KEYUP:
                        switch_stats = true;
                }
            }
        }
        if (switch_stats) {
            for (int i = 255; i >= 0; i -= 2) {
                SDL_RenderClear(renderer);
                SDL_SetTextureAlphaMod(title_texture, i);
                SDL_RenderCopy(renderer, title_texture, nullptr, &title_box);
                for (int j = 0; j < n_of_lines; ++j) {
                    SDL_SetTextureAlphaMod(text_textures[j], i);
                    text_box.w = static_cast<int>(780.0 * stats[j].size() / max_size);
                    text_box.x = 400 - text_box.w / 2;
                    text_box.y = 140 + j * text_height + 20 * (j >> 1);
                    SDL_RenderCopy(renderer, text_textures[j], nullptr, &text_box);
                }
                SDL_Delay(max(100.0 / 6.0 - static_cast<double>(SDL_GetTicks() - framerate_last_tick), 0.0));
                SDL_RenderPresent(renderer);
                framerate_last_tick = SDL_GetTicks();
            }
        }
    }
}

void Entity::render(SDL_Renderer *renderer, SDL_Texture *texture) const noexcept {
    const auto mob_crop = SDL_Rect(position[0] - radius, position[1] - radius, 2 * radius, 2 * radius);
    SDL_RenderCopy(renderer, texture, nullptr, &mob_crop);
}

double line_point_distance(int *line, const vector<int> &point) {
    return abs(line[0] * point[0] + line[1] * point[1] + line[2]) / sqrt(point[0] * point[0] + point[1] * point[1]);
}

vector<double> line_line_intersection(int *line1, int *line2) {
    int cramer_det = line1[0] * line2[1] - line1[1] * line2[0];
    if (cramer_det == 0) {
        return vector<double>{-1, -1};
    }
    return vector<double>{
            static_cast<double>(line2[1] * line1[2] - line2[2] * line1[1]) / cramer_det,
            static_cast<double>(line2[2] * line1[0] - line2[0] * line1[2]) / cramer_det
    };
}

double point_point_distance(const vector<int> &p1, const vector<int> &p2) {
    return sqrt((p1[0] - p2[0]) * (p1[0] - p2[0]) + (p1[1] - p2[1]) * (p1[1] - p2[1]));
}