#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Entity.h"
#include "Map.h"
#include "Util.h"
#include "Scene.h"

#include "Menu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"

#include "Win.h"
#include "Lose.h"

#include "stb_image.h" // only for start
#include <time.h>
using namespace std;

SDL_Window* display_window;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene* currentScene;
Scene* sceneList[6];

Mix_Music* music;
Mix_Chunk* hop;

void SwitchToScene(int _nextScene, int _lives = 3) {
    if (_nextScene == 1) {
        currentScene = new Level1(_lives);
    }
    else if (_nextScene == 2) {
        currentScene = new Level2(_lives);
    }
    else if (_nextScene == 3) {
        currentScene = new Level3(_lives);
    }

    // if win (4) or lose (5)
    else {
        currentScene = sceneList[_nextScene];
    }
    currentScene->Initialize();
}


const char START_SPRITE[] = "start.png";
GLuint start_texture_id;
glm::mat4 model_matrix_start;

// only for start page
GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);

    return textureID;
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    display_window = SDL_CreateWindow("Brave Knight", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(display_window);
    SDL_GL_MakeCurrent(display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    // start page (not menu!
    start_texture_id = load_texture(START_SPRITE);
    model_matrix_start = glm::mat4(1.0f);
    model_matrix_start = glm::scale(model_matrix_start, glm::vec3(10.0f, 10.0f, 5.0f));

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("file_example_MP3_700KB.mp3");
    Mix_PlayMusic(music, -1);

    hop = Mix_LoadWAV("spin_jump.wav");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // black
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    sceneList[0] = new Menu();
    sceneList[1] = new Level1(3);
    sceneList[2] = new Level2(3);
    sceneList[3] = new Level3(3);
    sceneList[4] = new Win();
    sceneList[5] = new Lose();
    SwitchToScene(0);
}

void ProcessInput() {

    currentScene->state.player->movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                // Move the player left
                break;

            case SDLK_RIGHT:
                // Move the player right
                break;

            case SDLK_RETURN:
                currentScene->state.player_lives = 3;
                SwitchToScene(1);
                break;

            case SDLK_SPACE:
                if (currentScene->state.player->collidedBottom) {
                    currentScene->state.player->jump = true;
                    Mix_PlayChannel(-1, hop, 0);
                }
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_RETURN]) {
        currentScene->state.player_lives = 3;
        SwitchToScene(1);
    }

    if (keys[SDL_SCANCODE_LEFT]) {
        currentScene->state.player->movement.x = -1.0f;
        //currentScene->state.player->animIndices = currentScene->state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        currentScene->state.player->movement.x = 1.0f;
        //currentScene->state.player->animIndices = currentScene->state.player->animRight;
    }


    if (glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
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
        currentScene->Update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;

    // ----------------- VIEW MATRIX -----------------
    viewMatrix = glm::mat4(1.0f);

    if (currentScene->state.player->position.y > -12.5) {
        if (currentScene->state.player->position.x > 5) {
            if (currentScene->state.player->position.x > 12) {
                if (currentScene->state.player->position.y > -2) {
                    viewMatrix = glm::translate(viewMatrix, glm::vec3(-12, 2.1, 0));
                }
                else {
                    viewMatrix = glm::translate(viewMatrix, glm::vec3(-12, -currentScene->state.player->position.y, 0));
                }
            }
            else {
                if (currentScene->state.player->position.y > -2) {
                    viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 2.1, 0));
                }
                else {
                    viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, -currentScene->state.player->position.y, 0));
                }
            }
        }
        else {
            if (currentScene->state.player->position.y > -2) {
                if (currentScene->state.player->position.x < 5) {
                    viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 2.1, 0));
                }
                else {
                    viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 2.1, 0));
                }
            }
            else {
                viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, -currentScene->state.player->position.y, 0));
            }
        }
    }
    else {
        if (currentScene->state.player->position.x < 5) {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 12.75, 0));
        }
        else {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 12.75, 0));
        }
    }

    std::cout << "(" << currentScene->state.player->position.x
        << ", " << currentScene->state.player->position.y;
    std::cout << ")\n";
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    program.SetViewMatrix(viewMatrix);

    currentScene->Render(&program);

    SDL_GL_SwapWindow(display_window);
}


void Shutdown() {
    SDL_Quit();
}

//below only for start page
void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id)
{
    program.SetModelMatrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}


void start_render() {

    glClear(GL_COLOR_BUFFER_BIT);

    //program.SetModelMatrix(model_matrix);

    // Vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);

    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(program.texCoordAttribute);

    draw_object(model_matrix_start, start_texture_id);

    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    SDL_GL_SwapWindow(display_window);
}
void sleepcp(int milliseconds) // Cross-platform sleep function
{
    clock_t time_end;
    time_end = clock() + milliseconds * CLOCKS_PER_SEC / 1000;
    while (clock() < time_end)
    {
    }
}

int main(int argc, char* argv[]) {
    Initialize();

   
    start_render();
    sleepcp(3500);

    while (gameIsRunning) {
        ProcessInput();
        Update();

        if (currentScene->state.nextScene >= 0) {
            if (currentScene->state.player_lives == 0) {
                SwitchToScene(5);
            }
            else {
                SwitchToScene(currentScene->state.nextScene, currentScene->state.player_lives);
            }
        }

        Render();
    }

    Shutdown();
    return 0;
}
