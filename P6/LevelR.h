#include "Scene.h"

class LevelR : public Scene {
public:
    int ENEMY_COUNT = 0;
    int EFFECT_COUNT = 0;

    LevelR(int player_lives);
    ~LevelR();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};