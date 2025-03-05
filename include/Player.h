#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>

class Player
{
public:
    Player(SDL_Renderer *renderer);
    ~Player();

    void handleInput(const Uint8 *keys);
    void update();
    void render();

private:
    SDL_Renderer *renderer;
    SDL_Texture *idleTexture;
    SDL_Texture *runTexture;

    enum State
    {
        IDLE,
        RUNNING
    } state;

    float posX, posY, velX, velY;
    const float speed = 3.0f;

    int frame;
    int frameTime;

    void loadTextures();
    SDL_Texture *loadTexture(const char *path);
};

#endif
