#include "Level1.h"

#define LEVEL1_WIDTH 18
#define LEVEL1_HEIGHT 17

#define LEVEL1_ENEMY_COUNT 1

unsigned int level1_data[] =
{
    
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 3,0, 0, 0, 0, 0, 0,
    
    3, 2, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 2, 0, 0, 2, 0, 3,
    3, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0,

    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 3, 0, 0, 0, 0,
    
    3, 0, 0, 0, 2, 2, 0, 0, 2, 2, 3, 0, 0, 0, 0, 0, 0, 3,
    3, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 2, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 3,
    
    3, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 2, 3, 0, 0, 3,
    3, 0, 0, 0, 3, 3, 0, 0, 2, 0, 0, 0, 2, 3, 0, 0, 0, 3,
    3, 0, 0, 3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 2, 2,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 0, 0, 3
};

Level1::Level1(int _lives) {
    state.player_lives = _lives;
}

void Level1::Initialize() {

    state.nextScene = -10;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);

    // ----------------- Initialize Player ------------------
    state.player = new Entity();
    state.player->entityType = PLAYER;

    state.player->position = glm::vec3(6, 0, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 2.5f;
    state.player->textureID = Util::LoadTexture("not_george.png");

    state.player->animRight = new int[4]{ 3, 7, 11, 15 };
    state.player->animLeft = new int[4]{ 1, 5, 9, 13 };
    state.player->animUp = new int[4]{ 2, 6, 10, 14 };
    state.player->animDown = new int[4]{ 0, 4, 8, 12 };

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;

    state.player->height = 0.8f;
    state.player->width = 0.35f;

    state.player->jumpPower = 5.0f;

    // ----------------- Initialize Enemies -----------------
    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];

    state.enemies[0].entityType = ENEMY;
    state.enemies[0].position = glm::vec3(9, -
        9, 0);
    state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);

    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;

    state.enemies[0].textureID = Util::LoadTexture("ai.png");
    state.enemies[0].height = 1.0f;
    state.enemies[0].width = 0.5f;
    state.enemies[0].movement = glm::vec3(0);
    state.enemies[0].speed = 1;
}

void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);

    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    }

    std::cout << "Lives: " << state.player_lives << "\n";

    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        std::cout << "Enemy: (" << state.enemies[i].position.x << ", ";
        std::cout << state.enemies[0].position.y << ") \n";

        if (state.player->CheckCollision(&state.enemies[i])) {
            if (state.enemies[i].collidedTop) {
                std::cout << "yuh\n";
                state.enemies[i].isActive = false;
            }
            else {
                std::cout << "ah!\n";
                loseLife();
                state.nextScene = 1;
            }
        }
    }

    if (state.player->position.x >= 15.85) {
        state.nextScene = 2;
    }
    // falling into pit
    else if (state.player->position.y < -15.5) {
        loseLife();
        if (state.player->lives == 0) {
            state.nextScene = 5;
        }
        else {
            state.nextScene = 1;
        }
    }
}

void Level1::Render(ShaderProgram* program) {
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    Util::DrawText(program, fontTextureID, "Level:1", 1.0f, 0.1f, glm::vec3(1, -12, 0));

    Util::DrawText(program, fontTextureID, "NOT Here~", 0.4f, 0.05f, glm::vec3(8.7, -11, 0));


    Util::DrawText(program, fontTextureID, "???", 0.6f, 0.1f, glm::vec3(15, 1.0, 0));

    // formatting lives
    std::string lives_str = std::to_string(state.player_lives);

    std::string rounded = lives_str.substr(0, lives_str.find(".") + 0);

    std::string lives_left = "Lives remaining: " + rounded;

    Util::DrawText(program, fontTextureID, lives_left, 0.4f, 0.1f, glm::vec3(1, 1.0, 0));

    //    state.enemies->Render(program); // why isn't this working rightttt
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }

    state.map->Render(program);
    state.player->Render(program);

}
