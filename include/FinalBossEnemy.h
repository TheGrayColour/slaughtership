// include/FinalBossEnemy.h
#ifndef FINAL_BOSS_ENEMY_H
#define FINAL_BOSS_ENEMY_H

#include "Enemy.h"
#include "Bullet.h"
#include <vector>
#include <SDL2/SDL.h>

class FinalBossEnemy : public Enemy
{
public:
    // match your Enemy ctor
    FinalBossEnemy(float x,
                   float y,
                   SDL_Renderer *renderer,
                   int variant);

    // override update to always patrol
    void update(float dt,
                const SDL_Rect & /*playerRect*/,
                const std::vector<SDL_Rect> &collisionTiles,
                std::vector<Bullet> &enemyBullets,
                bool /*playerAlive*/) override;
};

#endif // FINAL_BOSS_ENEMY_H
