#include <bits/stdc++.h>
#include "SDL.h"
#include "SDL_render.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

#ifndef SS_GAME_MAP_H

using namespace std;

enum Direction {
    UP, DOWN, LEFT, RIGHT, NONE
};

enum Sprite_type {
    WALL, EMPTY_BOX, CLOWN, KARATEKA, PROTAGONIST, PROTAGONIST_SHOT, ENEMY_SHOT
};

struct Entity {
    vector<int> position{0, 0};
    vector<int> velocity{0, 0};
    int radius{};
    int hp{};
    int id{};
    int8_t hit_tick{};
    int8_t action_tick{};
    Sprite_type type;
    int current_sprite;

    Entity() : type(EMPTY_BOX) {};

    Entity(const Entity &entity) = default;

    Entity(const vector<int> &position, int radius, int hp, int id, Sprite_type type) {
        this->position[0] = position[0];
        this->position[1] = position[1];
        this->velocity[0] = this->velocity[1] = 0;
        this->radius = radius;
        this->hp = hp;
        this->id = id;
        this->hit_tick = this->action_tick = 0;
        this->type = type;
        this->current_sprite = 0;
    }

    Entity& operator=(const Entity &entity) {
        this->position[0] = entity.position[0];
        this->position[1] = entity.position[1];
        this->velocity[0] = entity.velocity[0];
        this->velocity[1] = entity.velocity[1];
        this->radius = entity.radius;
        this->hp = entity.hp;
        this->id = entity.id;
        this->hit_tick = entity.hit_tick;
        this->action_tick = entity.action_tick;
        this->type = entity.type;
        this->current_sprite = entity.current_sprite;
        return *this;
    }

    void render(SDL_Renderer *renderer, SDL_Texture *texture) const noexcept;
};

struct Enemy : public Entity {
    double movement_average{};
    double movement_control{};

    Enemy() : Entity() {}

    Enemy(const vector<int> &position, int radius, int hp, int id, Sprite_type type,
          double movement_average, double movement_control) : Entity(position, radius, hp, id, type) {
        this->movement_average = movement_average;
        this->movement_control = movement_control;
    }
};

struct Floor_data {
    int protagonist_shots_fired{};
    int protagonist_shots_hit{};
    int protagonist_swings{};
    int protagonist_swings_hit{};
    int enemy_shots_fired{};
    int enemy_shots_hit{};
    int enemy_explosion_hits{};
    int enemy_contact_hits{};
};

struct Game_stats {
    int karateka_average_speed;
    double clown_shoot_probability;
};

struct Room_adaptations {
    double clown_shoot_probability;
    int karateka_average_speed;
    double karateka_probability;
    int average_number_of_enemies;
};

class General_handler {
    SDL_Window *window{};
    SDL_Renderer *renderer{};
    vector<vector<bool>> room;
    map<Sprite_type, vector<SDL_Texture *>> sprite_map{};
    map<int, int> sprite_clock{};
    map<int, int> karateka_kick_animation{};
    map<int, int> clown_explosion{};
    map<int, int> clown_charge{};
    vector<Entity> enemy_shots{};
    vector<Entity> protagonist_shots{};
    int protagonist_swing{};
    vector<int> protagonist_swing_direction{};
    SDL_Texture *swing_texture{};
    SDL_Texture *trapdoor_texture{};
    SDL_Texture *explosion_texture{};
    Mix_Chunk *karateka_kick_sound{};
    Mix_Chunk *clown_charge_sound{};
    Mix_Chunk *clown_explosion_sound{};
    Mix_Chunk *victory_sound{};
    Mix_Chunk *game_over_sound{};

    TTF_Font *isaac_font;
    TTF_Font *standard_font;

    mt19937_64 rng;
    normal_distribution<double> gauss = normal_distribution<double>();
    uniform_real_distribution<double> uniform_real = uniform_real_distribution<double>(0, 1);
    uniform_int_distribution<int> coin_flip = uniform_int_distribution<int>(0, 1);

    Uint32 framerate_last_tick = 0;
    bool pause = false;

    void shifted_render(const vector<vector<bool>> &new_room, int shift, Direction direction) noexcept;

    bool avoid_wall_collision(Entity &entity);

    bool collide(const map<int, vector<int>> &previous_positions, const Entity &entity1, const Entity &entity2);
public:
    int id_counter = 0;
    int game_time;
    int room_time;
    int total_n_of_enemies = 0;
    int total_n_of_karateka = 0;
    Entity protagonist;
    vector<Enemy> enemies{};
    bool final_room{};
    Game_stats game_stats{};
    Floor_data floor_data;
    vector<Room_adaptations> adaptations{};
    Mix_Music *background_music{};
    General_handler() = default;

    ~General_handler() {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        for (const auto &[type, texture_list] : sprite_map) {
            for (SDL_Texture *texture : texture_list) {
                SDL_DestroyTexture(texture);
            }
        }
        Mix_FreeMusic(background_music);
        Mix_FreeChunk(clown_explosion_sound);
        Mix_FreeChunk(clown_charge_sound);
        Mix_FreeChunk(karateka_kick_sound);
        Mix_FreeChunk(victory_sound);
        Mix_FreeChunk(game_over_sound);
        TTF_CloseFont(isaac_font);
    }

    void initialize(bool final_room);

    void room_change_animation(const vector<vector<bool>> &new_room, const vector<Enemy> &new_room_enemies, Direction);

    void base_render() noexcept;

    bool poll_events_and_update_positions() noexcept;

    void initial_timer() noexcept;

    void defeat_screen() noexcept;

    void victory_screen() noexcept;

    void stats_screen() noexcept;
};

double line_point_distance(int *line, const vector<int> &point);
vector<double> line_line_intersection(int *line1, int *line2);
double point_point_distance(const vector<int> &p1, const vector<int> &p2);

#define SS_GAME_MAP_H

#endif
