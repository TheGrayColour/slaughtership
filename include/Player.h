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
    float getWidth() const { return spriteWidth; }
    float getHeight() const { return spriteHeight; }
    float getAngle() const { return angle; }
    void setAngle(float newAngle) { angle = newAngle; }

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
    float angle = 0.0f;
    const float speed = 3.0f, bulletSpeed = 10.0f;
    float aimX, aimY;

    int frame;
    int frameTime;
    const int RUN_FRAMES = 12;
    const int IDLE_FRAMES = 1;
    const int FRAMES_SPEED = 3;

    const int spriteWidth = 64, spriteHeight = 64;

    void loadTextures();
    SDL_Texture *loadTexture(const char *path);
};

#endif
