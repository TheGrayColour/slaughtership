#include "BossEnemy.h"
#include "ResourceManager.h"
#include <iostream>

BossEnemy::BossEnemy(float x, float y, SDL_Renderer *renderer, int variant)
    : Enemy(x, y, renderer) // call base class constructor
{
    std::string base = "assets/enemies/boss" + std::to_string(variant) + "_";
    // Override enemy textures with boss textures.
    enemyIdleTexture = ResourceManager::loadTexture(renderer, base + "idle.png");
    enemyRunTexture = ResourceManager::loadTexture(renderer, base + "run.png");
    deadTexture = ResourceManager::loadTexture(renderer, base + "dead.png");

    // Optionally, adjust boss-specific parameters (e.g., health, speed) if desired.
    // For example:
    // setHealth(500);
    // setSpeed(30.0f);
}
