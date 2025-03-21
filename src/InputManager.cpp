#include "InputManager.h"
#include "Constants.h"

void InputManager::processInput(const Uint8 *keys, float &velX, float &velY, bool &isMoving)
{
    // Reset movement values.
    velX = 0.0f;
    velY = 0.0f;
    isMoving = false;

    // Process horizontal input.
    if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
    {
        velX = PLAYER_SPEED; // Use your player's speed or a defined constant.
        isMoving = true;
    }
    else if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
    {
        velX = -PLAYER_SPEED;
        isMoving = true;
    }

    // Process vertical input.
    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
    {
        velY = PLAYER_SPEED;
        isMoving = true;
    }
    else if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
    {
        velY = -PLAYER_SPEED;
        isMoving = true;
    }
}
