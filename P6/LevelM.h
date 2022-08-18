#include "Scene.h"

class LevelM : public Scene {
public:
    int ENEMY_COUNT = 0;
    int EFFECT_COUNT = 0;

    LevelM(int player_lives);
    ~LevelM();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};