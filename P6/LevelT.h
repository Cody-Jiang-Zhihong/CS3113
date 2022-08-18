#include "Scene.h"

class LevelT : public Scene {
public:
    int ENEMY_COUNT = 1;
    int EFFECT_COUNT = 3;

    LevelT(int player_lives);
    ~LevelT();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};