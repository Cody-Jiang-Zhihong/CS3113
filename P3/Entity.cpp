/*******************************
    NYU Tandon - CS 3113
    Project3 - Gundam Lander
       - Entity.cpp -
       By Cody Jiang
       zj2247@nyu.edu
*******************************/

#include "Entity.h"
#include <vector>

Entity::Entity()
{
    position = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    modelMatrix = glm::mat4(1.0f);
}

bool Entity::Collision_Detection(Entity* other) {
    if (is_active == false || other->is_active == false) return false;

    float x_distance = fabs(position.x - other->position.x) - ((width + other->width) / 2.0);
    float y_distance = fabs(position.y - other->position.y) - ((height + other->height) / 2.0);

    if (x_distance < 0 && y_distance < 0) return true;

    return false;
}

void Entity::Collision_Detection_Y(Entity* objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];

        if (Collision_Detection(object))
        {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));

            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                touch_Top = true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                touch_Bottom = true;
            }
        }
    }
}

void Entity::Collision_Detection_X(Entity* objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];

        if (Collision_Detection(object))
        {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                touch_Right = true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                touch_Left = true;
            }
        }
    }
}

void Entity::Update(float deltaTime, Entity* platforms, int platformCount)
{
    if (is_active == false) return;

    touch_Top = false;
    touch_Bottom = false;
    touch_Left = false;
    touch_Right = false;
    land_success = false;

    velocity.x = acceleration.x * speed;
    velocity += acceleration * deltaTime;

    position.x += velocity.x * deltaTime;
    Collision_Detection_X(platforms, platformCount); // check collisions

    position.y += velocity.y * deltaTime;
    Collision_Detection_Y(platforms, platformCount); // check collisions

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::Render(ShaderProgram* program) {
    if (is_active == false) return;

    program->SetModelMatrix(modelMatrix);

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
