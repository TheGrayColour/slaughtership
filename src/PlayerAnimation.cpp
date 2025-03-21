#include "PlayerAnimation.h"
#include <SDL2/SDL_image.h>
#include <iostream>

PlayerAnimation::PlayerAnimation(SDL_Renderer *renderer)
    : frame(0), frameTime(0), currentState(AnimationState::IDLE)
{
    idleTexture = loadTexture(renderer, "assets/player/player_idle.png");
    runTexture = loadTexture(renderer, "assets/player/player_run.png");
    attackTexture = loadTexture(renderer, "assets/player/player_punch.png");
}

PlayerAnimation::~PlayerAnimation()
{
    // Unique pointers auto-clean.
}

std::unique_ptr<SDL_Texture, SDLTextureDeleter> PlayerAnimation::loadTexture(SDL_Renderer *renderer, const char *path)
{
    SDL_Surface *surface = IMG_Load(path);
    if (!surface)
    {
        std::cerr << "Failed to load surface: " << path
                  << " SDL_Error: " << SDL_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!tex)
    {
        std::cerr << "Failed to create texture: " << path
                  << " SDL_Error: " << SDL_GetError() << std::endl;
        return nullptr;
    }
    return std::unique_ptr<SDL_Texture, SDLTextureDeleter>(tex);
}

void PlayerAnimation::triggerAttack()
{
    // Start an attack only if not already attacking.
    if (currentState != AnimationState::ATTACKING)
    {
        currentState = AnimationState::ATTACKING;
        frame = 0;
        frameTime = 0;
    }
}

void PlayerAnimation::update(bool isMoving)
{
    // If we are currently in ATTACKING state, continue the attack animation
    if (currentState == AnimationState::ATTACKING)
    {
        frameTime++;
        if (frameTime >= FRAME_SPEED)
        {
            frameTime = 0;
            if (frame < ATTACK_FRAMES - 1)
            {
                frame++;
            }
            else
            {
                // Attack animation finished; switch state based on movement.
                currentState = isMoving ? AnimationState::RUNNING : AnimationState::IDLE;
                frame = 0;
            }
        }
    }
    else
    {
        // Not attacking: set state based on movement.
        currentState = isMoving ? AnimationState::RUNNING : AnimationState::IDLE;
        frameTime++;
        if (frameTime >= FRAME_SPEED)
        {
            frameTime = 0;
            if (currentState == AnimationState::RUNNING)
            {
                frame = (frame + 1) % RUN_FRAMES;
            }
            else
            { // IDLE
                frame = 0;
            }
        }
    }
}

bool PlayerAnimation::isAnimationFinished() const
{
    // Attack animation is finished when in ATTACKING state and at the last frame.
    return (currentState == AnimationState::ATTACKING && frame >= ATTACK_FRAMES - 1);
}

void PlayerAnimation::render(SDL_Renderer *renderer, float x, float y, float angle)
{
    SDL_Texture *currentTex = idleTexture.get();
    const int spriteWidth = 54, spriteHeight = 54;

    switch (currentState)
    {
    case AnimationState::ATTACKING:
        if (attackTexture)
            currentTex = attackTexture.get();
        break;
    case AnimationState::RUNNING:
        if (runTexture)
            currentTex = runTexture.get();
        break;
    case AnimationState::IDLE:
    default:
        currentTex = idleTexture.get();
        break;
    }

    SDL_Rect srcRect = {frame * spriteWidth, 0, spriteWidth, spriteHeight};
    SDL_Rect destRect = {static_cast<int>(x), static_cast<int>(y), spriteWidth, spriteHeight};
    SDL_Point center = {spriteWidth / 2, spriteHeight / 2};

    SDL_RenderCopyEx(renderer, currentTex, &srcRect, &destRect, angle, &center, SDL_FLIP_NONE);
}

void PlayerAnimation::reset()
{
    frame = 0;
    frameTime = 0;
    // Do not change currentState here; let update() manage it.
}
