#include "Bullet.h"
#include <SDL2/SDL.h>

Bullet::Bullet(float x, float y, float dx, float dy, float speed)
    : x(x), y(y), dx(dx), dy(dy), speed(speed), active(true) {}

void Bullet::update(float dt, int screenWidth, int screenHeight)
{ // Use delta time (dt) for frame-rate independent movement.
    x += dx * speed * dt;
    y += dy * speed * dt;

    // Deactivate bullet if it goes off-screen
    if (x < 0 || x > screenWidth || y < 0 || y > screenHeight)
        active = false;
}

void Bullet::render(SDL_Renderer *renderer, int cameraX, int cameraY)
{
    SDL_Rect rect = {(int)(x - cameraX), (int)(y - cameraY), 5, 5};
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
}
