#include "BossEnemy.h"
#include "ResourceManager.h"
#include <iostream>

BossEnemy::BossEnemy(float x, float y, SDL_Renderer *renderer)
    : Enemy(x, y, renderer) // call base class constructor
{
    // Override enemy textures with boss textures.
    enemyIdleTexture = ResourceManager::loadTexture(renderer, "assets/enemies/boss_idle.png");
    enemyRunTexture = ResourceManager::loadTexture(renderer, "assets/enemies/boss_run.png");
    deadTexture = ResourceManager::loadTexture(renderer, "assets/enemies/boss_dead.png");

    // Optionally, adjust boss-specific parameters (e.g., health, speed) if desired.
    // For example:
    // setHealth(500);
    // setSpeed(30.0f);
}
