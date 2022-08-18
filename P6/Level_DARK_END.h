#include "Scene.h"

class LevelDE : public Scene {
public:
    int ENEMY_COUNT = 1;
    int EFFECT_COUNT = 3;
    int ITEM_COUNT = 1;

    LevelDE(int player_lives);
    ~LevelDE();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};