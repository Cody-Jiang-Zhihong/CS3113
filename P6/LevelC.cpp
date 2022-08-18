/*******************************
    NYU Tandon - CS 3113
    Project6 - God-Me
      - LevelC.cpp -
       By Cody Jiang
         8/18/2022
       zj2247@nyu.edu
*******************************/

#include "LevelC.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

unsigned int LEVEL_C_DATA[] =
{

    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 3,
    3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 3,
    3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 3, 3, 0, 0, 3, 3, 0, 3,
    3, 0, 0, 3, 3, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 3, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 2, 2, 0, 0, 0, 0, 2, 2, 3, 2, 0, 2, 0, 0, 2, 0, 3,
    3, 0, 0, 2, 0, 2, 0, 0, 3, 0, 0, 0, 0, 0, 2, 3, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 3, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 2, 0, 3, 3, 3, 0, 3,
    3, 0, 0, 0, 0, 2, 2, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 3,
    3, 2, 2, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 2, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 3, 0, 3, 3,
    3, 0, 0, 3, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 3, 3, 3, 3,
    3, 3, 0, 0, 3, 3, 0, 0, 2, 3, 3, 0, 2, 3, 0, 0, 0, 3,
    3, 3, 3, 0, 0, 0, 0, 3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 0, 0, 3

};

LevelC::LevelC(int player_lives) {
    state.player_lives = player_lives;
}

LevelC::~LevelC()
{
    delete[] this->state.enemies;
    delete    this->state.player;
    delete    this->state.map;
    Mix_FreeChunk(this->state.jump_sfx);
    Mix_FreeMusic(this->state.bgm);
}

void LevelC::initialise()
{
    state.next_scene_id = -10;

    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    this->state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_C_DATA, map_texture_id, 1.0f, 4, 1);

    // Code from main.cpp's initialise()
    /**
     Cody's Stuff
     */
     // Existing
    state.player = new Entity();
    state.player->set_entity_type(PLAYER);
    state.player->set_position(glm::vec3(1.0f, 0.0f, 0.0f));
    state.player->set_movement(glm::vec3(0.0f));
    state.player->speed = 2.5f;
    state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    state.player->texture_id = Utility::load_texture("assets/cody_0_full.png");

    // Walking
    /*
    state.player->walking[state.player->LEFT] = new int[4]{ 1, 5, 9,  13 };
    state.player->walking[state.player->RIGHT] = new int[4]{ 3, 7, 11, 15 };
    state.player->walking[state.player->UP] = new int[4]{ 2, 6, 10, 14 };
    state.player->walking[state.player->DOWN] = new int[4]{ 0, 4, 8,  12 };

    state.player->animation_indices = state.player->walking[state.player->RIGHT];  // start George looking Right
    */
    state.player->animation_left = new int[4]{ 1, 5, 9,  13 };
    state.player->animation_right = new int[4]{ 3, 7, 11, 15 };
    state.player->animation_up = new int[4]{ 2, 6, 10, 14 };
    state.player->animation_down = new int[4]{ 0, 4, 8,  12 };

    state.player->animation_indices = state.player->animation_right;  // start George looking Right

    state.player->animation_frames = 4;
    state.player->animation_index = 0;
    state.player->animation_time = 0.0f;
    state.player->animation_cols = 4;
    state.player->animation_rows = 4;
    state.player->set_height(0.8f);
    state.player->set_width(0.8f);

    // Jumping
    state.player->jumping_power = 5.0f;

    /**
     Enemies' stuff
     */
    state.enemies = new Entity[this->ENEMY_COUNT];
    GLuint enemy_texture_id = Utility::load_texture("assets/enemy_1.png");;
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].set_entity_type(ENEMY);

        if (i == 0) {
            state.enemies[i].set_position(glm::vec3(1.0f, -7.0f, 0)); // works
        }
        else if (i == 1) {
            state.enemies[i].set_position(glm::vec3(4.5f, -5.0f, 0));
        }
        else if (i == 2) {
            state.enemies[i].set_position(glm::vec3(0.0f, 10.0f, 0)); //scary one
        }

        state.enemies[i].acceleration = glm::vec3(0, -9.81f, 0);
        state.enemies[i].set_ai_type(GUARD);
        state.enemies[i].set_ai_state(IDLE);

        state.enemies[i].texture_id = enemy_texture_id;

        state.enemies[i].movement = glm::vec3(0);
        state.enemies[i].speed = 1;

        state.enemies[i].animation_left = new int[4]{ 1, 5, 9,  13 };
        state.enemies[i].animation_right = new int[4]{ 3, 7, 11, 15 };
        state.enemies[i].animation_up = new int[4]{ 2, 6, 10, 14 };
        state.enemies[i].animation_down = new int[4]{ 0, 4, 8,  12 };

        state.enemies[i].animation_indices = state.player->animation_left;  // start looking Right

        state.enemies[i].animation_frames = 4;
        state.enemies[i].animation_index = 0;
        state.enemies[i].animation_time = 0.0f;
        state.enemies[i].animation_cols = 4;
        state.enemies[i].animation_rows = 2;
        state.enemies[i].set_height(0.8f);
        state.enemies[i].set_width(0.8f);
    }

    /**
     Effects' stuff
     */
    GLuint effect_texture_id = Utility::load_texture("assets/effect_longrange_right.png");
    state.effects = new Entity[EFFECT_COUNT];

    for (int i = 0; i < EFFECT_COUNT; i++) {
        state.effects[i].set_entity_type(EFFECT);
        state.effects[i].set_effect_type(SHOOT);
        state.effects[i].texture_id = effect_texture_id;

        //state.effects[0].set_position(state.player->get_position());
        state.effects[i].set_movement(glm::vec3(0.0f));
        state.effects[i].speed = 4.0f;
        state.effects[i].set_position(glm::vec3(30.0f, 30.0f, 0.0f));
        state.effects[i].set_acceleration(glm::vec3(0.0f, 0.0, 0.0f));

        state.effects[i].animation_left = new int[1]{ 1 };
        state.effects[i].animation_right = new int[1]{ 2 };
        state.effects[i].animation_cols = 1;
        state.effects[i].animation_rows = 1;
        state.effects[i].set_height(0.8f);
        state.effects[i].set_width(0.8f);
    }


    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    state.bgm = Mix_LoadMUS("assets/file_example_MP3_700KB.mp3");
    Mix_PlayMusic(state.bgm, -1);
    Mix_VolumeMusic(100.0f);
}

void LevelC::update(float delta_time)
{
    state.player->update(delta_time, state.player, state.enemies, ENEMY_COUNT, state.map);
    state.player->is_exsit = true;

    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].update(delta_time, state.player, state.enemies, ENEMY_COUNT, state.map);
    }

    for (int i = 0; i < EFFECT_COUNT; i++) {
        state.effects[i].update(delta_time, state.effects, state.enemies, ENEMY_COUNT, state.map);
    }

    for (int i = 0; i < ENEMY_COUNT; i++) {

        if (state.player->check_collision(&state.enemies[i])) {
            lose_life();
            if (state.player_lives == 0) {
                state.next_scene_id = 10;
            }
            else {
                state.next_scene_id = 3;
            }

        }
    }

    int total_defeat = 0;
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (state.enemies[i].was_defeated) total_defeat += 1;
    }
    if (total_defeat == 3) state.next_scene_id = 4;

}

void LevelC::render(ShaderProgram* program)
{
    this->state.map->render(program);
    this->state.player->render(program);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].render(program);
    }

    for (int i = 0; i < EFFECT_COUNT; i++) {
        state.effects[i].render(program);
    }
}