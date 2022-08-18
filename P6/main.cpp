/*******************************
    NYU Tandon - CS 3113
    Project6 - God-Me
       - main.cpp -
       By Cody Jiang
         8/18/2022
       zj2247@nyu.edu
*******************************/

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
#include "Utility.h"
#include "Scene.h"

#include "LevelM.h"
#include "LevelT.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "LevelD.h"
#include "LevelE.h"
#include "LevelF.h"

#include "Level_LIGHT_END.h"
#include "Level_LIGHT_WIN.h"

#include "Level_DARK_END.h"

#include "LevelR.h"

#include "stb_image.h" // only for start

SDL_Window* displayWindow;
bool gameIsRunning = true;
//bool KnightMode_on = false;
bool enemy_1_talked = false;
bool speed_up = false;
int effect_num = 0;

bool light_mode_on = false;

ShaderProgram program;
glm::mat4 viewMatrix;
glm::mat4 modelMatrix;
glm::mat4 projectionMatrix;

Scene* currentScene;
Scene* sceneList[12];

Mix_Music* music;
Mix_Chunk* hop;

void SwitchScene(int next_scene_id, int lives = 8) {
    if (next_scene_id == 0) {
        currentScene = new LevelM(lives);
    }
    else if (next_scene_id == 1) {
        currentScene = new LevelA(lives);
    }
    else if (next_scene_id == 2) {
        currentScene = new LevelB(lives);
    }
    else if (next_scene_id == 3) {
        currentScene = new LevelC(lives);
    }
    else if (next_scene_id == 4) {
        currentScene = new LevelD(lives);
    }
    else if (next_scene_id == 5) {
        currentScene = new LevelE(lives);
    }
    else if (next_scene_id == 6) {
        currentScene = new LevelF(lives);
    }
    else if (next_scene_id == 7) {
        currentScene = new LevelLLE(lives);
    }
    else if (next_scene_id == 9) {
        currentScene = new LevelT(999);
    }
    else if(next_scene_id == 10){
        currentScene = new LevelR(999);
    }
    else if (next_scene_id == 11) {
        currentScene = new LevelLW(999);
    }
    else if (next_scene_id == 12) {
        currentScene = new LevelDE(999);
    }

    currentScene->initialise();
    
}

// below only for start page
char START_SPRITE[] = "assets/start.png";
GLuint start_texture_id;
glm::mat4 model_matrix_start;

char PLOT_SPRITE[] = "assets/plot_white.png";
GLuint plot_background_id;

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
// above only for start page

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("God-Me", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 960, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

    

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 1280, 960);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    // start page (not menu!
    start_texture_id = load_texture(START_SPRITE);
    plot_background_id = load_texture(PLOT_SPRITE);

    model_matrix_start = glm::mat4(1.0f);
    model_matrix_start = glm::scale(model_matrix_start, glm::vec3(10.0f, 10.0f, 5.0f));
    // start page (not menu!

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // black
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    sceneList[0] = new LevelA(3); // 3 is the lives
    SwitchScene(0);

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

            case SDLK_UP:
                // Move the player up
                break;

            case SDLK_DOWN:
                // Move the player down
                break;
                /*
            case SDLK_RETURN:
                currentScene->state.player_lives = 3;
                SwitchScene(1); // switch to levelA
                break;*/

            case SDLK_SPACE:
                if (currentScene->state.player->collided_bottom) {
                    currentScene->state.player->is_jumping = true;
                    Mix_PlayChannel(-1, Mix_LoadWAV("assets/spin_jump.wav"), 0);
                }
                break;
            }

            break; // SDL_KEYDOWN
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_T]) {
        currentScene->state.player_lives = 999;
        SwitchScene(9, currentScene->state.player_lives); // switch to level M - MENU
    }

    if (key_state[SDL_SCANCODE_RETURN]) {
        currentScene->state.player_lives = 3;
        SwitchScene(1); // switch to level A
    }

    //if (key_state[SDL_SCANCODE_LEFT]) {
    if (key_state[SDL_SCANCODE_A] && currentScene->state.player->is_exsit) {
        currentScene->state.player->movement.x = -1.0f;
        currentScene->state.player->animation_indices = currentScene->state.player->animation_left;
        if (currentScene->state.effects[0].effect_active == true) {
            currentScene->state.effects->animation_indices = currentScene->state.effects->animation_left;
            currentScene->state.effects[0].effect_active = false;
            if (currentScene->state.player->KnightMode_on) {
                currentScene->state.effects->texture_id = Utility::load_texture("assets/knight_longrange_left.png");
            }
            else {
                currentScene->state.effects->texture_id = Utility::load_texture("assets/effect_longrange_left.png");
            }
        }
    }
    //else if (key_state[SDL_SCANCODE_RIGHT]) {
    else if (key_state[SDL_SCANCODE_D] && currentScene->state.player->is_exsit) {
        currentScene->state.player->movement.x = 1.0f;
        currentScene->state.player->animation_indices = currentScene->state.player->animation_right;
        if (currentScene->state.effects[0].effect_active == true){
            currentScene->state.effects->animation_indices = currentScene->state.effects->animation_right;
            currentScene->state.effects[0].effect_active = false;
            if (currentScene->state.player->KnightMode_on) {
                currentScene->state.effects->texture_id = Utility::load_texture("assets/knight_longrange_right.png");
            }
            else {
                currentScene->state.effects->texture_id = Utility::load_texture("assets/effect_longrange_right.png");
            }
        }
    }
    //if (key_state[SDL_SCANCODE_UP])
    if (key_state[SDL_SCANCODE_W] && currentScene->state.player->is_exsit)
    {
        currentScene->state.player->movement.y = 1.0f;
        currentScene->state.player->animation_indices = currentScene->state.player->animation_up;
    }
    //else if (key_state[SDL_SCANCODE_DOWN])
    else if (key_state[SDL_SCANCODE_S] && currentScene->state.player->is_active)
    {
        currentScene->state.player->movement.y = -1.0f;
        currentScene->state.player->animation_indices = currentScene->state.player->animation_down;
    }
    // Feature 1: Switch Knight Mode
    else if (key_state[SDL_SCANCODE_G] && currentScene->state.player->is_exsit) {
        if (currentScene->state.player->KnightMode_on == false) {
            currentScene->state.player->texture_id = Utility::load_texture("assets/cody_1_full.png");
            currentScene->state.player->speed = 4.0f;
            currentScene->state.player->jumping_power = 7.0f;
            currentScene->state.player->KnightMode_on = true;
        }
        else {
            currentScene->state.player->texture_id = Utility::load_texture("assets/cody_0_full.png");
            currentScene->state.player->speed = 2.5f;;
            currentScene->state.player->jumping_power = 5.0f;
            currentScene->state.player->KnightMode_on = false;
        }
    }
    /*
    // Feature 2: NPC Interactions
    else if (key_state[SDL_SCANCODE_RETURN] && currentScene->state.player->check_collision_Talk(&currentScene->state.enemies[0]) && currentScene->state.player->is_exsit) {
        if (enemy_1_talked == false) {
            currentScene->state.enemies[0].texture_id = Utility::load_texture("assets/enemy_1_chat_1.png");
            enemy_1_talked = true;
        }
        else {
            currentScene->state.enemies[0].texture_id = Utility::load_texture("assets/enemy_1.png");
            enemy_1_talked = false;
        }
    }
    */

    // Feature 4: Attacks
    else if (key_state[SDL_SCANCODE_J] && currentScene->state.player->is_exsit)
    {
        currentScene->state.effects[0].effect_active = true;

        currentScene->state.effects[0].set_position(currentScene->state.player->get_position());
        //effect_num++;
    }
    else if (key_state[SDL_SCANCODE_R] && currentScene->state.player->is_exsit)
    {
        currentScene->state.effects[0].effect_active = false;
    }
    // Feature 5: Speed UP
    else if (key_state[SDL_SCANCODE_LSHIFT] || key_state[SDL_SCANCODE_RSHIFT] && currentScene->state.player->is_exsit)
    {
        if (!speed_up) {
            currentScene->state.player->speed = 5.0f;
            speed_up = true;
        }
        else {
            currentScene->state.player->speed = 2.5f;
            speed_up = false;
        }
    }
    //Secret key at ending of light
    else if (key_state[SDL_SCANCODE_L])
    {
        if (light_mode_on == false) {
            currentScene->state.player->texture_id = Utility::load_texture("assets/cody_light.png");
            currentScene->state.player->animation_cols = 2;
            currentScene->state.player->animation_rows = 1;
            light_mode_on = true;
        }
        else {
            currentScene->state.player->texture_id = Utility::load_texture("assets/cody_light_2.png");
            currentScene->state.player->animation_cols = 4;
            currentScene->state.player->animation_rows = 2;
        }
        
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
        currentScene->update(FIXED_TIMESTEP);
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

    currentScene->render(&program);

    SDL_GL_SwapWindow(displayWindow);
}

//below only for start page



void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id)
{
    program.SetModelMatrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void sleepcp(int milliseconds) // Cross-platform sleep function
{
    clock_t time_end;
    time_end = clock() + milliseconds * CLOCKS_PER_SEC / 1000;
    while (clock() < time_end)
    {
    }
}

void start_render() {

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_PlayMusic(Mix_LoadMUS("assets/The Ice Giants.mp3"), -1);
    Mix_VolumeMusic(100.0f);

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

    SDL_GL_SwapWindow(displayWindow);
    sleepcp(10000);
}

void plot_background_render() {

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

    draw_object(model_matrix_start, plot_background_id);


    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    SDL_GL_SwapWindow(displayWindow);
    sleepcp(15000);
}


int main(int argc, char* argv[]) {

    Initialize();

    start_render();

    plot_background_render();
    
    //currentScene->state.next_scene_id = 5; //just for test

	SDL_Event event;
	while (gameIsRunning) {

		ProcessInput();
		Update();

        if (currentScene->state.next_scene_id >= 0) {
            if (currentScene->state.player_lives == 0) {
                SwitchScene(10);
            }
            else {
                SwitchScene(currentScene->state.next_scene_id, currentScene->state.player_lives);
            }
        }


        Render();
	}

	SDL_Quit();

	return 0;
}
