#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <SDL2/SDL.h>

class InputManager
{
public:
    // Processes input keys and updates velocity and moving flag.
    // The speed values are set here; you might want to parameterize these or
    // use a constant defined elsewhere.
    static void processInput(const Uint8 *keys, float &velX, float &velY, bool &isMoving);
};

#endif // INPUT_MANAGER_H
