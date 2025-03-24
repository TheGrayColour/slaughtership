#ifndef COLLISION_HANDLER_H
#define COLLISION_HANDLER_H

#include <SDL2/SDL.h>
#include <vector>

class CollisionHandler
{
public:
    // Returns true if the given box intersects any wall.
    static bool checkCollision(const SDL_Rect &box, const std::vector<SDL_Rect> &walls)
    {
        for (const SDL_Rect &wall : walls)
        {
            if (SDL_HasIntersection(&box, &wall))
                return true;
        }
        return false;
    }
};

#endif // COLLISION_HANDLER_H
