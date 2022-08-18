/*******************************
    NYU Tandon - CS 3113
    Project6 - God-Me
      - LevelE.cpp -
       By Cody Jiang
         8/18/2022
       zj2247@nyu.edu
*******************************/

#include "LevelE.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8


unsigned int LEVEL_E_DATA[] =
{

    //3,3,//3,3
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

};


LevelE::LevelE(int player_lives) {
    state.player_lives = player_lives;
}

LevelE::~LevelE()
{
    delete[] this->state.enemies;
    delete    this->state.player;
    delete    this->state.map;
    Mix_FreeChunk(this->state.jump_sfx);
    Mix_FreeMusic(this->state.bgm);
}

void LevelE::initialise()
{
    state.next_scene_id = -10;

    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    this->state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_E_DATA, map_texture_id, 1.0f, 4, 1);

    // Code from main.cpp's initialise()

    /**
     Cody's Stuff
     */
     // Existing
    state.player = new Entity();
    state.player->set_entity_type(PLAYER);
    state.player->set_position(glm::vec3(1.0f, -3.0f, 0.0f));
    state.player->set_movement(glm::vec3(0.0f));
    state.player->speed = 4.0f;
    state.player->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    state.player->texture_id = Utility::load_texture("assets/cody_1_full.png");
    state.player->TWO_D_WALK = true;

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

    state.player->KnightMode_on = true;
    state.player->animation_indices = state.player->animation_right;  // start looking Right

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
    GLuint enemy_texture_id_1 = Utility::load_texture("assets/enemy_1.png");
    GLuint enemy_texture_id_2 = Utility::load_texture("assets/unknown_enemy_full.png");
    GLuint enemy_texture_id_3 = Utility::load_texture("assets/ydoc_1_full.png");

    state.enemies = new Entity[this->ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].set_entity_type(ENEMY);

        if (i == 0) {
            state.enemies[i].set_position(glm::vec3(6.0f, -1.0f, 0));
            state.enemies[i].texture_id = enemy_texture_id_3;
            state.enemies[i].animation_cols = 4;
            state.enemies[i].animation_rows = 4;
            state.enemies[i].set_ai_type(YDOC);
            state.enemies[i].set_ai_state(IDLE);
            state.enemies[i].TWO_D_WALK = true;
        }
        else if (i == 1) {
            state.enemies[i].set_position(glm::vec3(4.5f, -5.0f, 0));
            state.enemies[i].texture_id = enemy_texture_id_2;
            state.enemies[i].animation_cols = 4;
            state.enemies[i].animation_rows = 4;
            state.enemies[i].set_ai_type(YDOC);
            state.enemies[i].set_ai_state(IDLE);
            state.enemies[i].TWO_D_WALK = true;;
        }
        else if (i == 2) {
            state.enemies[i].set_position(glm::vec3(6.5f, -3.0f, 0));
            state.enemies[i].texture_id = enemy_texture_id_2;
            state.enemies[i].animation_cols = 4;
            state.enemies[i].animation_rows = 4;
            state.enemies[i].set_ai_type(YDOC);
            state.enemies[i].set_ai_state(IDLE);
            state.enemies[i].TWO_D_WALK = true;
        }
        else if (i == 3) {
            state.enemies[i].set_position(glm::vec3(5.0f, -8.0f, 0));
            state.enemies[i].texture_id = enemy_texture_id_2;
            state.enemies[i].animation_cols = 4;
            state.enemies[i].animation_rows = 4;
            state.enemies[i].set_ai_type(YDOC);
            state.enemies[i].set_ai_state(IDLE);
            state.enemies[i].TWO_D_WALK = true;
        }
        else if (i == 4) {
            state.enemies[i].set_position(glm::vec3(6.0f, -9.5f, 0));
            state.enemies[i].texture_id = enemy_texture_id_1;
            state.enemies[i].animation_cols = 4;
            state.enemies[i].animation_rows = 2;
            state.enemies[i].set_ai_type(GUARD);
            state.enemies[i].set_ai_state(IDLE);
        }
        else if (i == 5) {
            state.enemies[i].set_position(glm::vec3(7.5f, -5.5f, 0));
            state.enemies[i].texture_id = enemy_texture_id_1;
            state.enemies[i].animation_cols = 4;
            state.enemies[i].animation_rows = 2;
            state.enemies[i].set_ai_type(GUARD);
            state.enemies[i].set_ai_state(IDLE);
        }
        else if (i == 6) {
            state.enemies[i].set_position(glm::vec3(7.0f, -7.5f, 0));
            state.enemies[i].texture_id = enemy_texture_id_1;
            state.enemies[i].animation_cols = 4;
            state.enemies[i].animation_rows = 2;
            state.enemies[i].set_ai_type(GUARD);
            state.enemies[i].set_ai_state(IDLE);
        }
        else if (i == 7) {
            state.enemies[i].set_position(glm::vec3(5.0f, -4.0f, 0));
            state.enemies[i].texture_id = enemy_texture_id_1;
            state.enemies[i].animation_cols = 4;
            state.enemies[i].animation_rows = 2;
            state.enemies[i].set_ai_type(GUARD);
            state.enemies[i].set_ai_state(IDLE);
        }
        else if (i == 8) {
            state.enemies[i].set_position(glm::vec3(8.5f, -10.0f, 0));
            state.enemies[i].texture_id = enemy_texture_id_1;
            state.enemies[i].animation_cols = 4;
            state.enemies[i].animation_rows = 2;
            state.enemies[i].set_ai_type(GUARD);
            state.enemies[i].set_ai_state(IDLE);
        }
        else if (i == 9) {
            state.enemies[i].set_position(glm::vec3(8.0f, -4.5f, 0));
            state.enemies[i].texture_id = enemy_texture_id_1;
            state.enemies[i].animation_cols = 4;
            state.enemies[i].animation_rows = 2;
            state.enemies[i].set_ai_type(GUARD);
            state.enemies[i].set_ai_state(IDLE);
        }

        state.enemies[i].acceleration = glm::vec3(0, 0, 0);

        state.enemies[i].movement = glm::vec3(0);
        state.enemies[i].speed = 0.6;

        state.enemies[i].animation_left = new int[4]{ 1, 5, 9,  13 };
        state.enemies[i].animation_right = new int[4]{ 3, 7, 11, 15 };
        state.enemies[i].animation_up = new int[4]{ 2, 6, 10, 14 };
        state.enemies[i].animation_down = new int[4]{ 0, 4, 8,  12 };

        state.enemies[i].animation_indices = state.player->animation_left;  // start looking left

        state.enemies[i].animation_frames = 4;
        state.enemies[i].animation_index = 0;
        state.enemies[i].animation_time = 0.0f;
        
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

        state.effects[i].set_position(glm::vec3(30.0f, 30.0f, 0.0f));
        state.effects[i].set_movement(glm::vec3(0.0f));
        state.effects[i].speed = 4.0f;
        state.effects[i].set_acceleration(glm::vec3(0.0f, 0.0, 0.0f));

        state.effects[i].animation_left = new int[1]{ 1 };
        state.effects[i].animation_right = new int[1]{ 2 };
        state.effects[i].animation_cols = 1;
        state.effects[i].animation_rows = 1;
    }

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    state.bgm = Mix_LoadMUS("assets/file_example_MP3_700KB.mp3");
    Mix_PlayMusic(state.bgm, -1);
    Mix_VolumeMusic(100.0f);

}

void LevelE::update(float delta_time)
{
    state.player->update(delta_time, state.player, state.enemies, ENEMY_COUNT, state.map);
    state.player->is_exsit = true;



    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].update(delta_time, state.player, state.enemies, ENEMY_COUNT, state.map);
    }

    for (int i = 0; i < EFFECT_COUNT; i++) {
        state.effects[i].update(delta_time, state.effects, state.enemies, ENEMY_COUNT, state.map);
    }

    if (state.player->check_collision(&state.enemies[0])) {
        state.next_scene_id = 5;
    }

    for (int i = 0; i < ENEMY_COUNT; i++) {

        if (state.player->check_collision(&state.enemies[i])) {
            lose_life();
            if (state.player_lives == 0) {
                state.next_scene_id = 10;
            }
            else {
                state.next_scene_id = 5;
            }

        }
    }

    int total_defeat = 0;
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (state.enemies[i].was_defeated) total_defeat += 1;
    }
    if (total_defeat == ENEMY_COUNT) state.next_scene_id = 6; // go to ending level

}

void LevelE::render(ShaderProgram* program)
{
    GLuint fontTextureID = Utility::load_texture("assets/font1.png");
    Utility::draw_text(program, fontTextureID, "...", 0.3f, 0.0f, glm::vec3(3.0, -2.0, 0));

    this->state.map->render(program);
    this->state.player->render(program);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].render(program);
    }

    for (int i = 0; i < EFFECT_COUNT; i++) {
        state.effects[i].render(program);
    }

}