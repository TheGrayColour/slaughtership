// src/FinalBossEnemy.cpp
#include "FinalBossEnemy.h"
#include "ResourceManager.h"

FinalBossEnemy::FinalBossEnemy(float x,
                               float y,
                               SDL_Renderer *renderer,
                               int variant)
    : Enemy(x, y, renderer, variant)
{
    // load boss-specific textures
    enemyIdleTexture = ResourceManager::loadTexture(renderer, "assets/enemies/final_boss_idle.png");
    enemyRunTexture = ResourceManager::loadTexture(renderer, "assets/enemies/final_boss_run.png");
    deadTexture = ResourceManager::loadTexture(renderer, "assets/enemies/boss_dead.png");
}

void FinalBossEnemy::update(float dt,
                            const SDL_Rect & /*playerRect*/,
                            const std::vector<SDL_Rect> &collisionTiles,
                            std::vector<Bullet> &enemyBullets,
                            bool /*playerAlive*/)
{
    // 1) Remember existing bullets
    size_t before = enemyBullets.size();

    // 2) Call Enemy::update with a fake player location far off-screen
    SDL_Rect fakeFarAway{100000, 100000, 0, 0};
    Enemy::update(dt, fakeFarAway, collisionTiles, enemyBullets, /*playerAlive=*/true);

    // 3) Remove any bullets the boss spawned
    if (enemyBullets.size() > before)
        enemyBullets.erase(
            enemyBullets.begin() + before,
            enemyBullets.end());
}
