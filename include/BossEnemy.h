#ifndef BOSS_ENEMY_H
#define BOSS_ENEMY_H

#include "Enemy.h"

class BossEnemy : public Enemy
{
public:
    BossEnemy(float x, float y, SDL_Renderer *renderer);
};

#endif // BOSS_ENEMY_H
