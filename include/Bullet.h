#ifndef BULLET_H
#define BULLET_H

#include <SDL2/SDL.h>

class Bullet
{
public:
    Bullet(float x, float y, float dirX, float dirY, float speed);

    void update();
    void render(SDL_Renderer *renderer, int cameraX, int cameraY);

    bool isActive() const { return active; }

private:
    float x, y;
    float dirX, dirY; // Normalized direction
    float speed;
    bool active;
};

#endif // BULLET_H