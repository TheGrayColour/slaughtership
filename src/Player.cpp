#include "Player.h"
#include "InputManager.h"
#include "CollisionHandler.h" // New collision handling helper
#include <SDL2/SDL_image.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "Constants.h"

Player::Player(SDL_Renderer *renderer, Level *level)
    : velX(0), velY(0), isMoving(false),
      renderer(renderer), level(level),
      posX(400), posY(400),
      bareFistAttacking(false)
{
    collisionBox = {static_cast<int>(posX) + PLAYER_COLLISION_OFFSET_X,
                    static_cast<int>(posY) + PLAYER_COLLISION_OFFSET_Y,
                    PLAYER_COLLISION_WIDTH, PLAYER_COLLISION_HEIGHT};
    animation = std::make_unique<PlayerAnimation>(renderer);
    weapons = std::make_unique<PlayerWeapons>();

    // For a projectile weapon test:
    weapons->pickupWeapon(std::make_unique<ProjectileWeapon>(WeaponType::PISTOL, WEAPON_AMMO_PISTOL, WEAPON_FIRE_RATE_PISTOL, WEAPON_BULLET_SPEED_PISTOL, 10), renderer);

    // weapons->pickupWeapon(std::make_unique<MeleeWeapon>(
    //                           WeaponType::BASEBALL_BAT, WEAPON_FIRE_RATE_MELEE, 5),
    //                       renderer);
}

Player::~Player()
{
    // Smart pointers handle resource cleanup.
}

void Player::updateInput(const Uint8 *keys)
{
    InputManager::processInput(keys, velX, velY, isMoving);
    if (velX != 0.0f && velY != 0.0f)
    {
        velX *= 0.7071f;
        velY *= 0.7071f;
    }
}

void Player::shoot(int mouseX, int mouseY, int cameraX, int cameraY)
{
    float centerX = posX + spriteWidth / 2;
    float centerY = posY + spriteHeight / 2;
    float worldMouseX = mouseX + cameraX;
    float worldMouseY = mouseY + cameraY;

    if (weapons->hasWeapon())
    {
        weapons->shoot(bullets, centerX, centerY, worldMouseX, worldMouseY);
    }
    else
    {
        if (!animation->isAttacking())
            animation->triggerAttack();
    }
}

void Player::update(int screenWidth, int screenHeight)
{
    float newX = posX + velX;
    float newY = posY + velY;

    collisionBox.x = static_cast<int>(posX) + PLAYER_COLLISION_OFFSET_X;
    collisionBox.y = static_cast<int>(posY) + PLAYER_COLLISION_OFFSET_Y;

    // Delegate collision detection to CollisionHandler.
    if (!CollisionHandler::checkCollision({static_cast<int>(newX) + PLAYER_COLLISION_OFFSET_X,
                                           static_cast<int>(posY) + PLAYER_COLLISION_OFFSET_Y,
                                           PLAYER_COLLISION_WIDTH, PLAYER_COLLISION_HEIGHT},
                                          level->getCollisionTiles()))
    {
        posX = newX;
    }
    if (!CollisionHandler::checkCollision({static_cast<int>(posX) + PLAYER_COLLISION_OFFSET_X,
                                           static_cast<int>(newY) + PLAYER_COLLISION_OFFSET_Y,
                                           PLAYER_COLLISION_WIDTH, PLAYER_COLLISION_HEIGHT},
                                          level->getCollisionTiles()))
    {
        posY = newY;
    }

    weapons->update();
    animation->update(isMoving);

    // Update bullets using a fixed delta time (could be adjusted)
    for (auto &bullet : bullets)
        bullet.update(1.0, screenWidth, screenHeight);

    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                 [](const Bullet &b)
                                 { return !b.isActive(); }),
                  bullets.end());
}

void Player::render(SDL_Renderer *renderer, int cameraX, int cameraY)
{
    float renderX = posX - cameraX;
    float renderY = posY - cameraY;
    // If a weapon is held, use the attached animation versions.
    if (weapons->hasWeapon())
    {
        animation->renderAttached(renderer, renderX, renderY, angle);
    }
    else
    {
        animation->render(renderer, renderX, renderY, angle);
    }
    // Render weapon effects (fire animations, etc.) on top of the player.
    weapons->render(renderer, renderX, renderY, angle);

    for (auto &bullet : bullets)
        bullet.render(renderer, cameraX, cameraY);
}
