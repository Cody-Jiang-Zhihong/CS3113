#include "Scene.h"

class LevelLLE : public Scene {
public:
    int ENEMY_COUNT = 1;
    int EFFECT_COUNT = 3;

    LevelLLE(int player_lives);
    ~LevelLLE();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};