/*******************************
    NYU Tandon - CS 3113
    Project6 - God-Me
      - LevelD.cpp -
       By Cody Jiang
         8/18/2022
       zj2247@nyu.edu
*******************************/

#include "LevelD.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8


unsigned int LEVEL_D_DATA[] =
{

    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

};


LevelD::LevelD(int player_lives) {
    state.player_lives = player_lives;
}

LevelD::~LevelD()
{
    delete[] this->state.enemies;
    delete    this->state.player;
    delete    this->state.map;
    Mix_FreeChunk(this->state.jump_sfx);
    Mix_FreeMusic(this->state.bgm);
}

void LevelD::initialise()
{
    state.next_scene_id = -10;

    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    this->state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_D_DATA, map_texture_id, 1.0f, 4, 1);

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
    GLuint enemy_texture_id = Utility::load_texture("assets/ydoc_1_full.png");

    state.enemies = new Entity[this->ENEMY_COUNT];
    state.enemies[0].set_entity_type(ENEMY);
    state.enemies[0].set_ai_type(YDOC);
    state.enemies[0].set_ai_state(IDLE);
    state.enemies[0].texture_id = enemy_texture_id;
    state.enemies[0].set_position(glm::vec3(12.0f, -6.0f, 0.0f)); // at corner
    state.enemies[0].set_movement(glm::vec3(0.0f));
    state.enemies[0].speed = 2.0f;
    state.enemies[0].set_acceleration(glm::vec3(0.0f, -0.0f, 0.0f));
    state.enemies[0].TWO_D_WALK = true;

    state.enemies[0].animation_left = new int[4]{ 1, 5, 9,  13 };
    state.enemies[0].animation_right = new int[4]{ 3, 7, 11, 15 };
    state.enemies[0].animation_up = new int[4]{ 2, 6, 10, 14 };
    state.enemies[0].animation_down = new int[4]{ 0, 4, 8,  12 };

    state.enemies[0].animation_indices = state.player->animation_left;  // start looking left

    state.enemies[0].animation_frames = 4;
    state.enemies[0].animation_index = 0;
    state.enemies[0].animation_time = 0.0f;
    state.enemies[0].animation_cols = 4;
    state.enemies[0].animation_rows = 4;
    state.enemies[0].set_height(0.8f);
    state.enemies[0].set_width(0.8f);

    /**
     Effects' stuff
     */
    GLuint effect_texture_id = Utility::load_texture("assets/effect_longrange_right.png");

    state.effects = new Entity[EFFECT_COUNT];
    state.effects[0].set_entity_type(EFFECT);
    state.effects[0].set_effect_type(SHOOT);
    state.effects[0].texture_id = effect_texture_id;

    state.effects[0].set_position(glm::vec3(30.0f, 0.0f, 0.0f));
    state.effects[0].set_movement(glm::vec3(0.0f));
    state.effects[0].speed = 4.0f;
    state.effects[0].set_acceleration(glm::vec3(0.0f, 0.0, 0.0f));

    state.effects[0].animation_left = new int[1]{ 1 };
    state.effects[0].animation_right = new int[1]{ 2 };
    state.effects[0].animation_cols = 1;
    state.effects[0].animation_rows = 1;

    /**
     Items' stuff
     */
    GLuint item_texture_id = Utility::load_texture("assets/laptop.png");

    state.items = new Entity[ITEM_COUNT];
    state.items[0].set_entity_type(ITEM);
    state.items[0].texture_id = item_texture_id;

    state.items[0].set_position(glm::vec3(7.0f, -4.0f, 0.0f));
    state.items[0].set_movement(glm::vec3(0.0f));
    state.items[0].speed = 0.0f;
    state.items[0].set_acceleration(glm::vec3(0.0f, 0.0, 0.0f));

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    state.bgm = Mix_LoadMUS("assets/file_example_MP3_700KB.mp3");
    Mix_PlayMusic(state.bgm, -1);
    Mix_VolumeMusic(100.0f);

}

void LevelD::update(float delta_time)
{
    state.player->update(delta_time, state.player, state.enemies, ENEMY_COUNT, state.map);
    state.player->is_exsit = true;



    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].update(delta_time, state.player, state.enemies, ENEMY_COUNT, state.map);
    }

    for (int i = 0; i < EFFECT_COUNT; i++) {
        state.effects[i].update(delta_time, state.effects, state.enemies, ENEMY_COUNT, state.map);
    }

    for (int i = 0; i < ITEM_COUNT; i++) {
        state.items[i].update(delta_time, state.items, state.enemies, ENEMY_COUNT, state.map);
    }

    if (state.player->check_collision(&state.items[0])) {
        state.next_scene_id = 4;
    }

    if (state.player->check_collision(&state.enemies[0])) {
        state.next_scene_id = 5;
    }

}

void LevelD::render(ShaderProgram* program)
{
    GLuint fontTextureID = Utility::load_texture("assets/font1.png");
    Utility::draw_text(program, fontTextureID, "You want ESCAPE?", 0.3f, 0.0f, glm::vec3(3.0, -2.0, 0));
    Utility::draw_text(program, fontTextureID, "You want ESCAPE?", 0.5f, 0.0f, glm::vec3(2.0, -3.0, 0));

    Utility::draw_text(program, fontTextureID, "COME TO ME...", 0.4f, 0.0f, glm::vec3(1.0, -4, 0));

    Utility::draw_text(program, fontTextureID, "Who are YOU?", 0.7f, 0.0f, glm::vec3(2.0, -5.0, 0));
    Utility::draw_text(program, fontTextureID, "Who are YOU.....?", 0.3f, 0.0f, glm::vec3(1.0, -5.5, 0));

    this->state.map->render(program);
    this->state.player->render(program);

    this->state.enemies->render(program);
    //this->state.effects->render(program);
    this->state.items->render(program);

}