#include "Scene.h"

class LevelB : public Scene {
public:
    int ENEMY_COUNT = 1;
    int EFFECT_COUNT = 3;

    LevelB(int player_lives);
    ~LevelB();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};