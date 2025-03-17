#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <vector>
#include "Bullet.h"
#include "Level.h"
#include "Weapon.h"

class Player
{
public:
    Player(SDL_Renderer *renderer, Level *level);
    ~Player();

    void handleInput(const Uint8 *keys);
    void shoot(int mouseX, int mouseY, int cameraX, int cameraY);
    void update(int screenWidth, int screenHeight);
    void render(SDL_Renderer *renderer, int cameraX, int cameraY);

    void pickupWeapon();
    void dropWeapon();

    float getX() const { return posX; }
    float getY() const { return posY; }
    float getWidth() const { return spriteWidth; }
    float getHeight() const { return spriteHeight; }
    float getAngle() const { return angle; }
    void setAngle(float newAngle) { angle = newAngle; }

private:
    SDL_Renderer *renderer;
    Level *level; // Reference to the level for collision detection

    SDL_Texture *idleTexture;
    SDL_Texture *runTexture;
    SDL_Texture *barefistTexture; // Punching animation (8 frames)
    const int ATTACK_FRAMES = 8;  // 8-frame attack animation
    bool isAttacking = false;     // Track attack state
    int attackFrameTime = 0;      // Track attack speed
    SDL_Texture *runAttachedTexture;
    SDL_Texture *idleAttachedTexture;

    enum State
    {
        IDLE,
        RUNNING,
        ATTACKING
    } state;

    float posX, posY, velX, velY;
    float angle = 0.0f;
    const float speed = 3.0f, bulletSpeed = 10.0f;
    float aimX, aimY;

    int frame;
    int frameTime;
    const int RUN_FRAMES = 8;
    const int IDLE_FRAMES = 1;
    const int FRAMES_SPEED = 3;

    const int spriteWidth = 54, spriteHeight = 54;

    SDL_Rect collisionBox;

    void loadTextures();
    SDL_Texture *loadTexture(const char *path);

    bool checkCollision(float newX, float newY);

    Weapon *currentWeapon = nullptr;    // Pointer to equipped weapon (nullptr if barefist)
    std::vector<Weapon> droppedWeapons; // Stores dropped weapons
    std::vector<Bullet> bullets;
};

#endif
