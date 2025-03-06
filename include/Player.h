#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <vector>
#include <Bullet.h>

class Player
{
public:
    Player(SDL_Renderer *renderer);
    ~Player();

    void handleInput(const Uint8 *keys);
    void shoot(int mouseX, int mouseY, int cameraX, int cameraY);
    void update();
    void render(SDL_Renderer *renderer, int cameraX, int cameraY);
    float getX() const { return posX; }
    float getY() const { return posY; }

private:
    SDL_Renderer *renderer;
    SDL_Texture *idleTexture;
    SDL_Texture *runTexture;

    enum State
    {
        IDLE,
        RUNNING
    } state;

    std::vector<Bullet> bullets;

    float posX, posY, velX, velY;
    const float speed = 3.0f, bulletSpeed = 10.0f;
    float aimX, aimY;

    int frame;
    int frameTime;

    void loadTextures();
    SDL_Texture *loadTexture(const char *path);
};

#endif
