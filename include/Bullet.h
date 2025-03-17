#ifndef BULLET_H
#define BULLET_H

#include <SDL2/SDL.h>

class Bullet
{
public:
    Bullet(float x, float y, float dx, float dy, float speed);

    void update(int screenWidth, int screenHeight);
    void render(SDL_Renderer *renderer, int cameraX, int cameraY);

    bool isActive() const { return active; }

private:
    float x, y;
    float dx, dy; // Normalized direction
    float speed;
    bool active;
};

#endif // BULLET_H