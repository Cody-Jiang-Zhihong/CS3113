/*******************************
    NYU Tandon - CS 3113
    Project3 - Gundam Lander
       - main.cpp -
       By Cody Jiang
       zj2247@nyu.edu
*******************************/

#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"
#include <vector>

#define TOTAL_PLATFORMS_COUNT 13
#define PLATFORMS_COUNT 3

struct GameState {
    Entity* player;
    Entity* platforms;
};

GameState state;

SDL_Window* displayWindow;
bool game_is_running = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image.\n";
        assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);

    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Gundam Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // white background
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize Gundam Entity
    state.player = new Entity();
    state.player->position = glm::vec3(0, 4.5f, 0);
    state.player->acceleration = glm::vec3(0, -0.91f, 0);
    state.player->speed = 1.0f;
    state.player->textureID = LoadTexture("gundam_lander.png");

    // Initialize Base Tiles
    state.platforms = new Entity[TOTAL_PLATFORMS_COUNT + PLATFORMS_COUNT];
    GLuint base_textureID = LoadTexture("platformPack_tile040.png");
    GLuint safe__base_textureID = LoadTexture("platformPack_tile036.png");
    GLuint platform_textureID = LoadTexture("platformPack_tile044.png");

    float x_base_position = -4.5f;
    float x_position = -3.2f;
    float y_position = 1.7f;

    for (int i = 0; i < TOTAL_PLATFORMS_COUNT; i++) {
        // there are three bases are safe
        if (i > 5 && i < 9) {
            state.platforms[i].textureID = safe__base_textureID;
            state.platforms[i].position = glm::vec3(x_base_position, -3.25, 0);
            state.platforms[i].height = 0.35;
            x_base_position += 1;
        }
        // three platforms in the air
        else if (i > 9) {
            state.platforms[i].textureID = platform_textureID;
            state.platforms[i].position = glm::vec3(x_position, y_position, 0);
            x_position += 3.3;
            y_position -= 1;
        }
        else {
            // else are all bottom base
            state.platforms[i].textureID = base_textureID;
            state.platforms[i].position = glm::vec3(x_base_position, -3.25, 0);
            x_base_position += 1;
        }
    }

    for (int i = 0; i < (TOTAL_PLATFORMS_COUNT + PLATFORMS_COUNT); i++) {
        state.platforms[i].Update(0, NULL, 0);
    }
}

void ShowResult(ShaderProgram* program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;

    std::vector<float> vertices;
    std::vector<float> texCoords;

    for (int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;

        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            });

        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
            });

    } // end of for loop

    glm::mat4 fontModelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(fontModelMatrix, position);
    program->SetModelMatrix(fontModelMatrix);

    glUseProgram(program->programID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void ProcessInput() {

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            game_is_running = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                // Move the player left
                break;

            case SDLK_RIGHT:
                // Move the player right
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (state.player->touch_Bottom == false) {
        if (keys[SDL_SCANCODE_LEFT]) {
            state.player->acceleration.x += -0.01f;
            state.player->acceleration.y = -0.1f; // Smoother moving feel, if delete, fall faster when press right or left
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            state.player->acceleration.x += 0.01f;
            state.player->acceleration.y = -0.1f; // Smoother moving feel
        }
        else if (state.player->acceleration.x != 0){
            // when you let go of the key, the ship should still drift for a bit
            if (state.player->acceleration.x > 0) {
                state.player->acceleration.x -= 0.01f;
            }
            else
                state.player->acceleration.x += 0.01f;
            
        }
    }
    else if ((state.player->Collision_Detection(&state.platforms[6]))
        || (state.player->Collision_Detection(&state.platforms[7]))
        || (state.player->Collision_Detection(&state.platforms[8]))) {

        state.player->land_success = true;
        std::cout << "You win!\n";
    }
    else {
        state.player->land_fail = true;
        std::cout << "Game Over!\n";
    }
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP) {
        state.player->Update(FIXED_TIMESTEP, state.platforms, TOTAL_PLATFORMS_COUNT);
        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < TOTAL_PLATFORMS_COUNT; i++) {
        state.platforms[i].Render(&program);
    }

    if (state.player->land_success == true) {
        ShowResult(&program, LoadTexture("font1.png"), "Mission Success!", 0.5f, -0.25f, glm::vec3(0, 0, 0));
        state.player->is_active = false;
    }
    else if (state.player->land_fail == true) {
        ShowResult(&program, LoadTexture("font1.png"), "Mission Fail!", 0.5f, -0.25f, glm::vec3(0, 0, 0));
        state.player->is_active = false;
    }

    state.player->Render(&program);

    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}


int main(int argc, char* argv[]) {
    Initialize();

    while (game_is_running) {
        ProcessInput();
        Update();
        Render();
    }

    Shutdown();
    return 0;
}
