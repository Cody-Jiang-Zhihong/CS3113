#include "Scene.h"

class LevelF : public Scene {
public:
    int ENEMY_COUNT = 2;
    int EFFECT_COUNT = 1;
    int ITEM_COUNT = 0;

    LevelF(int player_lives);
    ~LevelF();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};