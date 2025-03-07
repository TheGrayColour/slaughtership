#include "Player.h"
#include <SDL2/SDL_image.h>
#include <iostream>
#include <cmath>
#include <algorithm>

Player::Player(SDL_Renderer *renderer)
    : renderer(renderer), state(IDLE), posX(400), posY(300), velX(0), velY(0), frame(0), frameTime(0)
{
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
}

SDL_Texture *Player::loadTexture(const char *path)
{
    SDL_Surface *tempSurface = IMG_Load(path);
    if (!tempSurface)
    {
        std::cerr << "Failed to load texture: " << IMG_GetError() << std::endl;
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

void Player::shoot(int mouseX, int mouseY, int cameraX, int cameraY)
{
    // Calculate the center of the player sprite
    float centerX = posX + 32; // Assuming 64x64 sprite, so +32 centers it
    float centerY = posY + 32;
    float worldMouseX = mouseX + cameraX;
    float worldMouseY = mouseY + cameraY;

    // Calculate direction vector
    float dx = worldMouseX - centerX;
    float dy = worldMouseY - centerY;
    float length = sqrt(dx * dx + dy * dy);

    if (length == 0)
        return; // Prevent division by zero

    dx /= length;
    dy /= length;

    bullets.emplace_back(centerX, centerY, dx, dy, bulletSpeed);
}

void Player::update()
{
    posX += velX;
    posY += velY;

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

    for (auto &bullet : bullets)
        bullet.update();

    // Remove inactive bullets
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                 [](const Bullet &b)
                                 { return !b.isActive(); }),
                  bullets.end());
}

void Player::render(SDL_Renderer *renderer, int cameraX, int cameraY)
{
    SDL_Rect srcRect = {frame * 64, 0, 64, 64}; // Get current frame
    SDL_Rect destRect = {(int)(posX - cameraX), (int)(posY - cameraY), spriteWidth, spriteHeight};
    SDL_Point rotationCenter = {spriteWidth / 2, spriteHeight / 2};

    SDL_Texture *currentTexture = (state == RUNNING) ? runTexture : idleTexture;
    SDL_RenderCopyEx(renderer, currentTexture, &srcRect, &destRect, angle, &rotationCenter, SDL_FLIP_NONE);

    // Render bullets
    for (auto &bullet : bullets)
        bullet.render(renderer, cameraX, cameraY);
}
