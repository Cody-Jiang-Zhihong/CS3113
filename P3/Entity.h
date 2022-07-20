/*******************************
    NYU Tandon - CS 3113
    Project3 - Gundam Lander
       - Entity.h -
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

class Entity {
public:
    glm::vec3 position;
    glm::vec3 acceleration;
    glm::vec3 velocity;

    float speed;

    float width = 1;
    float height = 1;

    GLuint textureID;

    glm::mat4 modelMatrix;

    bool is_active = true;

    bool touch_Top = false;
    bool touch_Bottom = false;
    bool touch_Left = false;
    bool touch_Right = false;
    bool land_success = false;
    bool land_fail = false;

    Entity();

    bool Collision_Detection(Entity* other);
    void Collision_Detection_X(Entity* objects, int objectCount);
    void Collision_Detection_Y(Entity* objects, int objectCount);
    void Update(float deltaTime, Entity* platforms, int platformCount);
    void Render(ShaderProgram* program);
};
