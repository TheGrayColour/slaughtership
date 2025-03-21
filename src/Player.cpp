#include "Player.h"
#include "InputManager.h" // For processing keyboard input
#include <SDL2/SDL_image.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "Constants.h"

Player::Player(SDL_Renderer *renderer, Level *level)
    : renderer(renderer), level(level),
      posX(400), posY(400), velX(0), velY(0), isMoving(false),
      bareFistAttacking(false)
{
    // Initialize the collision box with offsets.
    collisionBox = {static_cast<int>(posX) + PLAYER_COLLISION_OFFSET_X,
                    static_cast<int>(posY) + PLAYER_COLLISION_OFFSET_Y,
                    PLAYER_COLLISION_WIDTH, PLAYER_COLLISION_HEIGHT};

    // Initialize separate components for animation and weapons.
    animation = std::make_unique<PlayerAnimation>(renderer);
    weapons = std::make_unique<PlayerWeapons>();
}

Player::~Player()
{
    // Unique pointers automatically clean up resources.
}

void Player::updateInput(const Uint8 *keys)
{
    // Delegate input processing to the InputManager.
    InputManager::processInput(keys, velX, velY, isMoving);

    // Normalize diagonal movement speed
    if (velX != 0.0f && velY != 0.0f)
    {
        velX *= 0.7071f; // 1/sqrt(2)
        velY *= 0.7071f;
    }
}

void Player::shoot(int mouseX, int mouseY, int cameraX, int cameraY)
{
    float centerX = posX + PLAYER_SPRITE_WIDTH / 2;
    float centerY = posY + PLAYER_SPRITE_HEIGHT / 2;
    float worldMouseX = mouseX + cameraX;
    float worldMouseY = mouseY + cameraY;

    if (weapons->hasWeapon())
    {
        weapons->shoot(bullets, centerX, centerY, worldMouseX, worldMouseY);
    }
    else
    {
        // Trigger bare-fist attack via animation.
        if (!animation->isAttacking())
            animation->triggerAttack();
    }
}

bool Player::checkCollision(float newX, float newY)
{
    SDL_Rect futureBox = {static_cast<int>(newX) + PLAYER_COLLISION_OFFSET_X,
                          static_cast<int>(newY) + PLAYER_COLLISION_OFFSET_Y,
                          PLAYER_COLLISION_WIDTH, PLAYER_COLLISION_HEIGHT};
    const auto &walls = level->getCollisionTiles();
    for (const SDL_Rect &wall : walls)
    {
        if (SDL_HasIntersection(&futureBox, &wall))
            return true;
    }
    return false;
}

void Player::update(int screenWidth, int screenHeight)
{
    float newX = posX + velX;
    float newY = posY + velY;

    collisionBox.x = static_cast<int>(posX) + PLAYER_COLLISION_OFFSET_X;
    collisionBox.y = static_cast<int>(posY) + PLAYER_COLLISION_OFFSET_Y;

    if (!checkCollision(newX, posY))
        posX = newX;
    if (!checkCollision(posX, newY))
        posY = newY;

    weapons->update();

    // Update the animation without passing an external attack flag.
    animation->update(isMoving);

    // (The weapon component will handle its own attack animation; if no weapon, the bare-fist attack is managed solely by animation.)

    for (auto &bullet : bullets)
        bullet.update(screenWidth, screenHeight);

    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                 [](const Bullet &b)
                                 { return !b.isActive(); }),
                  bullets.end());
}

void Player::render(SDL_Renderer *renderer, int cameraX, int cameraY)
{
    // Render the player's animation.
    animation->render(renderer, posX - cameraX, posY - cameraY, angle);

    // Render the weapons (both held and dropped).
    weapons->render(renderer, cameraX, cameraY);

    // Render bullets.
    for (auto &bullet : bullets)
        bullet.render(renderer, cameraX, cameraY);
}
