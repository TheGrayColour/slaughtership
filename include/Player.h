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
    void handleMouseInput(const SDL_Event &event);
    void shoot(int mouseX, int mouseY);
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

    std::vector<Bullet> bullets;

    float posX, posY, velX, velY;
    const float speed = 3.0f, bulletSpeed = 10.0f;
    float aimX, aimY; 
    bool shooting; 

    int frame;
    int frameTime;

    void loadTextures();
    SDL_Texture *loadTexture(const char *path);
};

#endif
