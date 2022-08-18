/*******************************
    NYU Tandon - CS 3113
    Project6 - God-Me
      - LevelF.cpp -
       By Cody Jiang
         8/18/2022
       zj2247@nyu.edu
*******************************/

#include "LevelF.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8


unsigned int LEVEL_F_DATA[] =
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


LevelF::LevelF(int player_lives) {
    state.player_lives = player_lives;
}

LevelF::~LevelF()
{
    delete[] this->state.enemies;
    delete    this->state.player;
    delete    this->state.map;
    Mix_FreeChunk(this->state.jump_sfx);
    Mix_FreeMusic(this->state.bgm);
}

void LevelF::initialise()
{
    state.next_scene_id = -10;

    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    this->state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_F_DATA, map_texture_id, 1.0f, 4, 1);

    // Code from main.cpp's initialise()

    /**
     Cody's Stuff
     */
     // Existing
    state.player = new Entity();
    state.player->set_entity_type(PLAYER);
    state.player->set_position(glm::vec3(6.0f, -2.0f, 0.0f));
    state.player->set_movement(glm::vec3(0.0f));
    state.player->speed = 2.5f;
    state.player->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    state.player->texture_id = Utility::load_texture("assets/whoscody_full.png");
    state.player->TWO_D_WALK = true;

    state.player->animation_left = new int[4]{ 1, 5, 9,  13 };
    state.player->animation_right = new int[4]{ 3, 7, 11, 15 };
    state.player->animation_up = new int[4]{ 2, 6, 10, 14 };
    state.player->animation_down = new int[4]{ 0, 4, 8,  12 };

    state.player->animation_indices = state.player->animation_down;  // start looking down

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
    GLuint enemy_texture_id_1 = Utility::load_texture("assets/unknown_enemy_full.png");
    GLuint enemy_texture_id_2 = Utility::load_texture("assets/ydoc_1_full_samesize.png");

    state.enemies = new Entity[this->ENEMY_COUNT];
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].set_entity_type(ENEMY);

        if (i == 0) {
            state.enemies[i].set_position(glm::vec3(2.0f, -4.0f, 0));
            state.enemies[i].texture_id = enemy_texture_id_1;
        }
        else if (i == 1) {
            state.enemies[i].set_position(glm::vec3(10.0f, -4.0f, 0));
            state.enemies[i].texture_id = enemy_texture_id_2;
        }

        state.enemies[i].animation_cols = 4;
        state.enemies[i].animation_rows = 4;
        state.enemies[i].set_ai_type(YDOC);
        state.enemies[i].set_ai_state(IDLE);
        //state.enemies[i].TWO_D_WALK = true;;

        state.enemies[i].acceleration = glm::vec3(0, 0, 0);

        state.enemies[i].movement = glm::vec3(0);
        state.enemies[i].speed = 0;

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

void LevelF::update(float delta_time)
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
        state.next_scene_id = 7;
    }
    
    if (state.player->check_collision(&state.enemies[1])) {
        state.next_scene_id = 12;
    }

}

void LevelF::render(ShaderProgram* program)
{
    GLuint fontTextureID = Utility::load_texture("assets/font1.png");
    Utility::draw_text(program, fontTextureID, "You finally got here.", 0.25f, 0.0f, glm::vec3(3, -1.5, 0));
    Utility::draw_text(program, fontTextureID, "But all the way to battle", 0.25f, 0.0f, glm::vec3(2.5, -2, 0));
    Utility::draw_text(program, fontTextureID, "your armor has been tornand tattered", 0.25f, 0.0f, glm::vec3(1.5, -2.5, 0));
    Utility::draw_text(program, fontTextureID, "Your sword has lost its fighting spirit", 0.25f, 0.0f, glm::vec3(1.5, -3, 0));
    Utility::draw_text(program, fontTextureID, "your consciousness is blurred", 0.25f, 0.0f, glm::vec3(2, -3.5, 0));
    Utility::draw_text(program, fontTextureID, "Don't we look alike now?", 0.4f, 0.0f, glm::vec3(1, -4.5, 0));
    Utility::draw_text(program, fontTextureID, "Left or right, make your choice", 0.25f, 0.0f, glm::vec3(2, -6, 0));


    this->state.map->render(program);
    this->state.player->render(program);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].render(program);
    }

    for (int i = 0; i < EFFECT_COUNT; i++) {
        state.effects[i].render(program);
    }

}