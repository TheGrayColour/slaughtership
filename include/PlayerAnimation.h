#ifndef PLAYER_ANIMATION_H
#define PLAYER_ANIMATION_H

#include <SDL2/SDL.h>
#include <memory>
#include "SDLDeleters.h"

enum class AnimationState
{
    IDLE,
    RUNNING,
    ATTACKING
};

class PlayerAnimation
{
public:
    PlayerAnimation(SDL_Renderer *renderer);
    ~PlayerAnimation();

    // Update animation based on external input: isMoving and an externally triggered attack flag.
    // Note: The isAttacking parameter should be true only in the frame when the attack is triggered.
    void update(bool isMoving);

    // Render the current frame at position (x,y) with rotation.
    void render(SDL_Renderer *renderer, float x, float y, float angle);

    // Reset animation state (e.g., when an attack is initiated)
    void reset();

    // Trigger an attack: sets the state to ATTACKING and resets the frame counter.
    void triggerAttack();

    // Return whether the attack animation has finished (i.e. when the last attack frame is reached).
    bool isAnimationFinished() const;

    // Expose current attack state.
    bool isAttacking() const { return currentState == AnimationState::ATTACKING; }

private:
    std::unique_ptr<SDL_Texture, SDLTextureDeleter> idleTexture;
    std::unique_ptr<SDL_Texture, SDLTextureDeleter> runTexture;
    std::unique_ptr<SDL_Texture, SDLTextureDeleter> attackTexture;

    int frame;
    int frameTime;

    const int FRAME_SPEED = 3;
    const int IDLE_FRAMES = 1;
    const int RUN_FRAMES = 8;
    const int ATTACK_FRAMES = 8;

    AnimationState currentState;

    std::unique_ptr<SDL_Texture, SDLTextureDeleter> loadTexture(SDL_Renderer *renderer, const char *path);
};

#endif // PLAYER_ANIMATION_H
