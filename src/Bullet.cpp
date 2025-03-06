#include "Bullet.h"
#include <SDL2/SDL.h>

Bullet::Bullet(float x, float y, float dirX, float dirY, float speed)
    : x(x), y(y), dirX(dirX), dirY(dirY), speed(speed), active(true) {}

void Bullet::update()
{
    x += dirX * speed;
    y += dirY * speed;

    // Deactivate bullet if it goes off-screen
    if (x < 0 || x > 800 || y < 0 || y > 600)
        active = false;
}

void Bullet::render(SDL_Renderer *renderer, int cameraX, int cameraY)
{
    SDL_Rect rect = {(int)(x - cameraX), (int)(y - cameraY), 5, 5};
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
}
