#include "Scene.h"

class LevelC : public Scene {
public:
    int ENEMY_COUNT = 3;
    int EFFECT_COUNT = 3;

    LevelC(int player_lives);
    ~LevelC();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};