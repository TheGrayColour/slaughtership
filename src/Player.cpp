#include "Player.h"
#include <SDL2/SDL_image.h>
#include <iostream>

Player::Player(SDL_Renderer *renderer)
    : renderer(renderer), state(IDLE), frame(0), frameTime(0), posX(400), posY(300), velX(0), velY(0)
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

void Player::update()
{
    posX += velX;
    posY += velY;

    frameTime++;
    int maxFrames = (state == RUNNING) ? 9 : 5; // Run has 9 frames, Idle has 5

    if (frameTime >= 10)
    { // Adjust speed of animation
        frame = (frame + 1) % maxFrames;
        frameTime = 0;
    }
}

void Player::render()
{
    SDL_Rect srcRect = {frame * 64, 0, 64, 64}; // Get current frame
    SDL_Rect destRect = {(int)posX, (int)posY, 64, 64};

    SDL_Texture *currentTexture = (state == RUNNING) ? runTexture : idleTexture;
    SDL_RenderCopy(renderer, currentTexture, &srcRect, &destRect);
}
