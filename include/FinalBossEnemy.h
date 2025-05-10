#ifndef FINAL_BOSS_ENEMY_H
#define FINAL_BOSS_ENEMY_H

#include "Enemy.h"

class FinalBossEnemy : public Enemy
{
public:
    FinalBossEnemy(float x, float y, SDL_Renderer *renderer);
};

#endif // FINAL_BOSS_ENEMY_H
