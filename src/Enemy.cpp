#include "Enemy.h"
#include "ResourceManager.h"
#include <cmath>
#include <iostream>
#include <SDL2/SDL.h>

// Constructor: load enemy textures and initialize state.
Enemy::Enemy(float x, float y, SDL_Renderer *renderer)
    : x(x), y(y), speed(50.0f), health(100), state(EnemyState::PATROLLING), angle(0),
      deathFrame(0), deathFrameTime(0), deathAnimationPlayed(false), deadEffectFrame(0), deadEffectFrameTime(0), deadEffectDelayCounter(0), fireTimer(0.0f)
{
    // Load textures using ResourceManager.
    enemyIdleTexture = ResourceManager::loadTexture(renderer, "assets/enemies/enemy_idle.png");
    enemyRunTexture = ResourceManager::loadTexture(renderer, "assets/enemies/enemy_run.png");
    deadTexture = ResourceManager::loadTexture(renderer, "assets/enemies/enemy_dead.png");

    weapon = std::make_unique<ProjectileWeapon>(WeaponType::SHOTGUN, WEAPON_AMMO_SHOTGUN, WEAPON_FIRE_RATE_SHOTGUN, WEAPON_BULLET_SPEED_SHOTGUN, 10);
    weapon->initialize(renderer);

    collisionBox.x = static_cast<int>(x) + PLAYER_COLLISION_OFFSET_X;
    collisionBox.y = static_cast<int>(y) + PLAYER_COLLISION_OFFSET_Y;
    collisionBox.w = PLAYER_COLLISION_WIDTH;
    collisionBox.h = PLAYER_COLLISION_HEIGHT;
}

void Enemy::update(float dt, const SDL_Rect &playerRect, const std::vector<SDL_Rect> &walls, std::vector<Bullet> &enemyBullets, bool playerAlive)
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

        if (playerAlive)
        { // Only attack if player is alive.

            fireTimer += dt;
            // Only attack if enough time has passed.
            if (fireTimer >= weapon->getFireRate())
            {
                attack(enemyBullets, playerRect);
                fireTimer = 0.0f;
            }
        }
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

    if (weapon)
        weapon->update();
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
        // Render death effect (unchanged or updated as needed)
        if (deadEffectDelayCounter < deadEffectDelayThreshold)
        {
            deadEffectDelayCounter++;
        }
        else
        {
            SDL_Texture *effectTex = ResourceManager::loadTexture(renderer, "assets/effect.png");
            if (effectTex)
            {
                // Adjust effect dimensions if desired (here assuming effect remains 98x54)
                const int effectWidth = 98, effectHeight = 54;
                SDL_Rect effectSrc = {deadEffectFrame * effectWidth, 0, effectWidth, effectHeight};
                SDL_Rect effectDest;
                // Center the effect relative to the enemy's original 54x54 hitbox:
                effectDest.x = static_cast<int>(x) - cameraX - (effectWidth - 54) / 2;
                effectDest.y = static_cast<int>(y) - cameraY;
                effectDest.w = effectWidth;
                effectDest.h = effectHeight;

                float rad = angle * M_PI / 180.0f;
                int offsetX = static_cast<int>(-33 * cos(rad));
                int offsetY = static_cast<int>(-33 * sin(rad));
                effectDest.x += offsetX;
                effectDest.y += offsetY;
                SDL_Point effectCenter = {effectWidth / 2, effectHeight / 2};
                SDL_RenderCopyEx(renderer, effectTex, &effectSrc, &effectDest, angle, &effectCenter, SDL_FLIP_NONE);

                deadEffectFrameTime++;
                if (deadEffectFrameTime >= DEAD_EFFECT_SPEED)
                {
                    if (deadEffectFrame < DEAD_EFFECT_FRAMES - 1)
                        deadEffectFrame++;
                    deadEffectFrameTime = 0;
                }
            }
        }

        // Render dead enemy sprite with new dimensions (100x54, 7 frames):
        SDL_Rect src;
        src.x = deathFrame * 100; // 100 pixels width per frame.
        src.y = 0;
        src.w = 100;
        src.h = 54;
        // Create a destination rectangle that centers the 100x54 image over the enemy’s original 54x54 hitbox.
        SDL_Rect destDead;
        destDead.x = static_cast<int>(x) - cameraX - (100 - 54) / 2; // Shift horizontally.
        destDead.y = static_cast<int>(y) - cameraY;                  // Adjust vertical position if needed.
        destDead.w = 100;
        destDead.h = 54;
        SDL_Point centerDead = {50, 27}; // Center of 100x54 image.
        SDL_RenderCopyEx(renderer, deadTexture, &src, &destDead, angle, &centerDead, SDL_FLIP_NONE);
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

    int screenX = static_cast<int>(x) - cameraX;
    int screenY = static_cast<int>(y) - cameraY;
    if (state != EnemyState::DEAD && weapon)
    {
        weapon->render(renderer, static_cast<float>(screenX), static_cast<float>(screenY), angle, false);
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

void Enemy::attack(std::vector<Bullet> &enemyBullets, const SDL_Rect &playerRect)
{
    // Calculate player's center.
    float playerCenterX = playerRect.x + playerRect.w / 2.0f;
    float playerCenterY = playerRect.y + playerRect.h / 2.0f;

    // Use the enemy's angle (already computed in update) to offset the bullet spawn.
    // Here, we choose an offset of 30 pixels from the enemy's (x,y).
    float spawnX = x + 27;
    float spawnY = y + 27;

    if (weapon && weapon->hasAmmo())
    {
        weapon->shoot(enemyBullets, spawnX, spawnY, playerCenterX, playerCenterY);
    }
}

std::unique_ptr<AbstractWeapon> Enemy::dropWeapon()
{
    if (weapon)
    {
        // Set the dropped weapon’s position to the enemy's center.
        // Adjust by 27 (half of 54) assuming the enemy sprite is 54x54.
        weapon->setPosition(x + 27, y + 27);
    }
    return std::move(weapon);
}
