#include "Enemy.h"
#include "ResourceManager.h"
#include <cmath>
#include <iostream>
#include <SDL2/SDL.h>

// Constructor: load enemy textures and initialize state.
Enemy::Enemy(float x, float y, SDL_Renderer *renderer)
    : x(x), y(y), speed(50.0f), health(100), state(EnemyState::PATROLLING), angle(0),
      deathFrame(0), deathFrameTime(0), deathAnimationPlayed(false)
{
    // Load textures using ResourceManager.
    enemyIdleTexture = ResourceManager::loadTexture(renderer, "assets/enemies/enemy_idle.png");
    enemyRunTexture = ResourceManager::loadTexture(renderer, "assets/enemies/enemy_run.png");
    deadTexture = ResourceManager::loadTexture(renderer, "assets/enemies/enemy_dead.png");

    collisionBox.x = static_cast<int>(x) + PLAYER_COLLISION_OFFSET_X;
    collisionBox.y = static_cast<int>(y) + PLAYER_COLLISION_OFFSET_Y;
    collisionBox.w = PLAYER_COLLISION_WIDTH;
    collisionBox.h = PLAYER_COLLISION_HEIGHT;
}

void Enemy::update(float dt, const SDL_Rect &playerRect, const std::vector<SDL_Rect> &walls)
{
    if (state == EnemyState::DEAD)
    {
        if (!deathAnimationPlayed)
        {
            deathFrameTime++;
            if (deathFrameTime >= DEATH_FRAME_SPEED)
            {
                deathFrame++;
                deathFrameTime = 0;
                if (deathFrame >= DEATH_FRAMES)
                {
                    deathFrame = DEATH_FRAMES - 1;
                    deathAnimationPlayed = true;
                }
            }
        }
        return;
    }

    // Calculate center positions.
    float enemyCenterX = x + 27; // assuming enemy sprite is 54x54
    float enemyCenterY = y + 27;
    float playerCenterX = playerRect.x + playerRect.w / 2;
    float playerCenterY = playerRect.y + playerRect.h / 2;

    // Compute distance to the player.
    float dx = playerCenterX - enemyCenterX;
    float dy = playerCenterY - enemyCenterY;
    float distance = std::sqrt(dx * dx + dy * dy);

    // If player is within detection range, switch to ATTACKING state.
    if (distance < 200.0f)
    {
        state = EnemyState::ATTACKING;
        // Face the player.
        angle = std::atan2(dy, dx) * (180.0f / M_PI);
        // For now, remain stationary when attacking.
    }
    else
    {
        state = EnemyState::PATROLLING;
        patrol(dt, walls);

        // Update run animation for patrolling enemies.
        runFrameTime++;
        if (runFrameTime >= RUN_FRAME_SPEED)
        {
            runFrame = (runFrame + 1) % RUN_FRAMES;
            runFrameTime = 0;
        }
    }
}

void Enemy::patrol(float dt, const std::vector<SDL_Rect> &walls)
{
    float newX = x + speed * dt;
    SDL_Rect newBox = collisionBox;
    newBox.x = static_cast<int>(newX) + PLAYER_COLLISION_OFFSET_X;
    if (!CollisionHandler::checkCollision(newBox, walls))
    {
        x = newX;
        collisionBox.x = static_cast<int>(x) + PLAYER_COLLISION_OFFSET_X;
    }
    else
    {
        speed = -speed; // Reverse direction on collision.
    }
    angle = (speed > 0) ? 0 : 180;
}

void Enemy::render(SDL_Renderer *renderer, int cameraX, int cameraY)
{
    SDL_Rect dest;
    dest.x = static_cast<int>(x) - cameraX;
    dest.y = static_cast<int>(y) - cameraY;
    dest.w = 54;
    dest.h = 54;
    SDL_Point center = {27, 27};

    if (state == EnemyState::DEAD)
    {
        // Use deadTexture sprite sheet for death animation.
        SDL_Rect src;
        src.x = deathFrame * 54;
        src.y = 0;
        src.w = 54;
        src.h = 54;
        SDL_RenderCopy(renderer, deadTexture, &src, &dest);
    }
    else if (state == EnemyState::PATROLLING)
    {
        // Use run texture animation:
        SDL_Rect src;
        src.x = runFrame * 54;
        src.y = 0;
        src.w = 54;
        src.h = 54;
        SDL_RenderCopyEx(renderer, enemyRunTexture, &src, &dest, angle, &center, SDL_FLIP_NONE);
    }
    else // For ATTACKING, use idle texture.
    {
        SDL_RenderCopyEx(renderer, enemyIdleTexture, nullptr, &dest, angle, &center, SDL_FLIP_NONE);
    }
}

void Enemy::takeDamage(int damage)
{
    health -= damage;
    if (health <= 0)
    {
        state = EnemyState::DEAD;
        // Optionally, trigger a death animation here.
        std::cout << "Enemy died at position (" << x << ", " << y << ")\n";
    }
}
