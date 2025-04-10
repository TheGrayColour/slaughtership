#include "PlayerAnimation.h"
#include <SDL2/SDL_image.h>
#include <iostream>
#include <cmath>

PlayerAnimation::PlayerAnimation(SDL_Renderer *renderer)
    : frame(0), frameTime(0), currentState(AnimationState::IDLE), deathFrame(0), deathFrameTime(0), rightHand(true)
{
    idleTexture = loadTexture(renderer, "assets/player/player_idle.png");
    runTexture = loadTexture(renderer, "assets/player/player_run.png");
    attackTexture = loadTexture(renderer, "assets/player/player_punch.png");

    attachedIdleTexture = loadTexture(renderer, "assets/player/player_idle_attached.png");
    attachedRunTexture = loadTexture(renderer, "assets/player/player_run_attached.png");

    deadTexture = loadTexture(renderer, "assets/player/player_dead.png");

    legsTexture = loadTexture(renderer, "assets/player/player_legs.png");
    legsFrame = 0;
    legsFrameTime = 0;

    deadEffectTexture = loadTexture(renderer, "assets/effect.png");
    deadEffectFrame = 0;
    deadEffectFrameTime = 0;
    deadEffectDelayCounter = 0;
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

        rightHand = !rightHand;
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

    if (isMoving)
    {
        legsFrameTime++;
        if (legsFrameTime >= LEGS_FRAME_SPEED)
        {
            legsFrame = (legsFrame + 1) % LEGS_FRAMES;
            legsFrameTime = 0;
        }
    }
    else
    {
        legsFrame = 0;
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
    SDL_RendererFlip flip = SDL_FLIP_NONE; // default

    switch (currentState)
    {
    case AnimationState::ATTACKING:
        if (attackTexture)
            currentTex = attackTexture.get();
        // If rightHand is false, flip horizontally to simulate left punch.
        if (!rightHand)
            flip = SDL_FLIP_VERTICAL;
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

    SDL_RenderCopyEx(renderer, currentTex, &srcRect, &destRect, angle, &center, flip);
}

void PlayerAnimation::renderAttached(SDL_Renderer *renderer, float x, float y, float angle)
{
    SDL_Texture *currentTex = attachedIdleTexture.get();
    const int spriteWidth = 54, spriteHeight = 54;
    SDL_RendererFlip flip = SDL_FLIP_NONE;

    switch (currentState)
    {
    case AnimationState::ATTACKING:
        if (attackTexture)
            currentTex = attackTexture.get();
        if (!rightHand)
            flip = SDL_FLIP_VERTICAL;
        break;
    case AnimationState::RUNNING:
        if (attachedRunTexture)
            currentTex = attachedRunTexture.get();
        break;
    case AnimationState::IDLE:
    default:
        currentTex = attachedIdleTexture.get();
        break;
    }

    SDL_Rect srcRect = {frame * spriteWidth, 0, spriteWidth, spriteHeight};
    SDL_Rect destRect = {static_cast<int>(x), static_cast<int>(y), spriteWidth, spriteHeight};
    SDL_Point center = {spriteWidth / 2, spriteHeight / 2};
    SDL_RenderCopyEx(renderer, currentTex, &srcRect, &destRect, angle, &center, flip);
}

void PlayerAnimation::renderDead(SDL_Renderer *renderer, float x, float y, float angle)
{

    renderDeathEffect(renderer, x, y, angle);

    // Assume deadTexture is a sprite sheet with 8 frames (54x54 each).
    SDL_Rect srcRect = {deathFrame * 100, 0, 100, 54};
    SDL_Rect destRect = {static_cast<int>(x) - (100 - 54) / 2, static_cast<int>(y), 100, 54};
    SDL_Point center = {50, 27};
    SDL_RenderCopyEx(renderer, deadTexture.get(), &srcRect, &destRect, angle, &center, SDL_FLIP_NONE);

    // Advance death animation until last frame is reached.
    deathFrameTime++;
    if (deathFrameTime >= DEATH_FRAME_SPEED)
    {
        if (deathFrame < DEATH_FRAMES - 1)
        {
            deathFrame++;
        }
        deathFrameTime = 0;
    }
}

void PlayerAnimation::renderLegs(SDL_Renderer *renderer, float x, float y, float legsAngle)
{
    if (!legsTexture)
        return;
    const int spriteWidth = 54, spriteHeight = 54;
    SDL_Rect srcRect = {legsFrame * spriteWidth, 0, spriteWidth, spriteHeight};
    SDL_Rect destRect = {static_cast<int>(x), static_cast<int>(y), spriteWidth, spriteHeight};
    SDL_Point center = {spriteWidth / 2, spriteHeight / 2};
    SDL_RenderCopyEx(renderer, legsTexture.get(), &srcRect, &destRect, legsAngle, &center, SDL_FLIP_NONE);
}

void PlayerAnimation::renderDeathEffect(SDL_Renderer *renderer, float x, float y, float angle)
{
    if (!deadEffectTexture)
        return;

    // Only start drawing the effect after the delay threshold.
    if (deadEffectDelayCounter < deadEffectDelayThreshold)
    {
        deadEffectDelayCounter++;
        return; // Skip drawing until delay is reached.
    }

    const int effectWidth = 98, effectHeight = 54;
    SDL_Rect srcRect = {deadEffectFrame * effectWidth, 0, effectWidth, effectHeight};
    // Center the effect under the 54x54 player sprite:
    SDL_Rect destRect = {static_cast<int>(x) - (effectWidth - 54) / 2, static_cast<int>(y), effectWidth, effectHeight};
    float rad = angle * M_PI / 180.0f;
    int offsetX = static_cast<int>(-33 * cos(rad));
    int offsetY = static_cast<int>(-33 * sin(rad));
    destRect.x += offsetX;
    destRect.y += offsetY;
    SDL_Point center = {effectWidth / 2, effectHeight / 2};
    SDL_RenderCopyEx(renderer, deadEffectTexture.get(), &srcRect, &destRect, angle, &center, SDL_FLIP_NONE);

    deadEffectFrameTime++;
    if (deadEffectFrameTime >= DEAD_EFFECT_SPEED)
    {
        if (deadEffectFrame < DEAD_EFFECT_FRAMES - 1)
            deadEffectFrame++;
        deadEffectFrameTime = 0;
    }
}

void PlayerAnimation::reset()
{
    frame = 0;
    frameTime = 0;
    // Do not change currentState here; let update() manage it.
}
