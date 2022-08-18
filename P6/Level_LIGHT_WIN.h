#include "Scene.h"

class LevelLW : public Scene {
public:
    int ENEMY_COUNT = 1;
    int EFFECT_COUNT = 3;
    int ITEM_COUNT = 1;

    LevelLW(int player_lives);
    ~LevelLW();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};