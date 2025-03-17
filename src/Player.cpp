#include "Player.h"
#include <SDL2/SDL_image.h>
#include <iostream>
#include <cmath>
#include <algorithm>

Player::Player(SDL_Renderer *renderer, Level *level)
    : renderer(renderer), level(level), state(IDLE), posX(400), posY(400), velX(0), velY(0), frame(0), frameTime(0), currentWeapon(nullptr)
{
    // 19 = (54 - 16) / 2 → Centers the 16px wide hitbox horizontally.
    // 11 = (54 - 32) / 2 → Centers the 32px tall hitbox verticall
    collisionBox = {(int)posX + 19, (int)posY + 19, 16, 16};

    loadTextures();
}

Player::~Player()
{
    SDL_DestroyTexture(idleTexture);
    SDL_DestroyTexture(runTexture);
}

void Player::loadTextures()
{
    idleTexture = loadTexture("assets/player/player_idle.png");
    runTexture = loadTexture("assets/player/player_run.png");

    idleAttachedTexture = loadTexture("assets/player/player_idle_attached.png");
    runAttachedTexture = loadTexture("assets/player/player_run_attached.png");

    barefistTexture = loadTexture("assets/player/player_punch.png");
}

SDL_Texture *Player::loadTexture(const char *path)
{
    SDL_Surface *tempSurface = IMG_Load(path);
    if (!tempSurface)
    {
        //
        return nullptr;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    return texture;
}

void Player::handleInput(const Uint8 *keys)
{
    bool moving = false;

    if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
    {
        velX = speed;
        moving = true;
    }
    else if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
    {
        velX = -speed;
        moving = true;
    }
    else
        velX = 0;

    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
    {
        velY = speed;
        moving = true;
    }
    else if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
    {
        velY = -speed;
        moving = true;
    }
    else
        velY = 0;

    state = moving ? RUNNING : IDLE;
}

bool Player::checkCollision(float newX, float newY)
{
    SDL_Rect futureBox = {(int)newX + 19, (int)newY + 19, 16, 16};

    for (const SDL_Rect &wall : level->getCollisionTiles())
    {
        if (SDL_HasIntersection(&futureBox, &wall))
        {
            return true; // Collision detected
        }
    }
    return false; // No collision
}

void Player::shoot(int mouseX, int mouseY, int cameraX, int cameraY)
{
    float centerX = posX + spriteWidth / 2;
    float centerY = posY + spriteHeight / 2;
    float worldMouseX = mouseX + cameraX;
    float worldMouseY = mouseY + cameraY;

    if (currentWeapon)
    {
        if (currentWeapon->isMelee())
        {
            currentWeapon->attack();
        }
        else
        {
            currentWeapon->shoot(bullets, centerX, centerY, worldMouseX, worldMouseY);
        }
    }
    else
    {
        // Play barefist attack animation
        state = ATTACKING;
        isAttacking = true;
        frame = 0; // Reset animation frame
        attackFrameTime = 0;
    }
}

void Player::pickupWeapon()
{
    if (!droppedWeapons.empty())
    {
        currentWeapon = &droppedWeapons.front();
        currentWeapon->initialize(renderer);          // Load textures when picked up
        droppedWeapons.erase(droppedWeapons.begin()); // Remove from world
    }
}

void Player::dropWeapon()
{
    if (currentWeapon)
    {
        droppedWeapons.push_back(*currentWeapon);
        droppedWeapons.back().setPosition(posX, posY);
        currentWeapon = nullptr;
    }
}

void Player::update(int screenWidth, int screenHeight)
{
    float newX = posX + velX;
    float newY = posY + velY;

    collisionBox.x = (int)posX + 19;
    collisionBox.y = (int)posY + 19;

    // Only move if no collision is detected
    if (!checkCollision(newX, posY))
    {
        posX = newX;
    }
    if (!checkCollision(posX, newY))
    {
        posY = newY;
    }

    if (currentWeapon)
    {
        currentWeapon->update(); // Update weapon attack animation
    }

    if (isAttacking)
    {
        attackFrameTime++;
        if (attackFrameTime >= FRAMES_SPEED)
        {
            frame++;
            attackFrameTime = 0;
        }

        if (frame >= ATTACK_FRAMES)
        {
            isAttacking = false;
            frame = 0;
        }
    }
    else
    {

        if (state == RUNNING)
        {
            frameTime++;
            if (frameTime >= FRAMES_SPEED)
            { // Adjust speed of animation
                frame = (frame + 1) % RUN_FRAMES;
                frameTime = 0;
            }
        }
        else
        {
            frame = 0;
        }
    }

    // Update bullets
    for (auto &bullet : bullets)
        bullet.update(screenWidth, screenHeight);

    // Remove inactive bullets
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                 [](const Bullet &b)
                                 { return !b.isActive(); }),
                  bullets.end());
}

void Player::render(SDL_Renderer *renderer, int cameraX, int cameraY)
{
    SDL_Rect srcRect = {frame * spriteWidth, 0, spriteWidth, spriteHeight}; // Get the current animation frame
    SDL_Rect destRect = {(int)(posX - cameraX), (int)(posY - cameraY), spriteWidth, spriteHeight};
    SDL_Point rotationCenter = {spriteWidth / 2, spriteHeight / 2};

    SDL_Texture *currentTexture = nullptr;
    if (isAttacking)
    {
        currentTexture = barefistTexture;
    }
    else if (currentWeapon)
    {
        currentTexture = (state == RUNNING) ? runAttachedTexture : idleAttachedTexture;
    }
    else
    {
        currentTexture = (state == RUNNING) ? runTexture : idleTexture;
    }

    SDL_RenderCopyEx(renderer, currentTexture, &srcRect, &destRect, angle, &rotationCenter, SDL_FLIP_NONE);

    // Render weapon on top of player
    if (currentWeapon)
    {
        if (currentWeapon && currentWeapon->isMelee() && currentWeapon->isCurrentlyAttacking())
        {
            currentWeapon->renderAttack(renderer, posX - cameraX, posY - cameraY);
        }
        else
        {
            SDL_Rect weaponSrc = {frame * spriteWidth, 0, spriteWidth, spriteHeight};
            SDL_Rect weaponDest = destRect;
            SDL_RenderCopyEx(renderer, currentWeapon->getHeldTexture(), &weaponSrc, &weaponDest, angle, &rotationCenter, SDL_FLIP_NONE);
        }
    }

    // Render dropped weapons
    for (auto &weapon : droppedWeapons)
    {
        weapon.render(renderer, weapon.getX() - cameraX, weapon.getY() - cameraY);
    }

    for (auto &bullet : bullets)
    {
        bullet.render(renderer, cameraX, cameraY);
    }
}
