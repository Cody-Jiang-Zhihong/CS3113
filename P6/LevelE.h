#include "Scene.h"

class LevelE : public Scene {
public:
    int ENEMY_COUNT = 10;
    int EFFECT_COUNT = 99;
    int ITEM_COUNT = 1;

    LevelE(int player_lives);
    ~LevelE();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};