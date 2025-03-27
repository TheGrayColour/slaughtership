#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "Bullet.h"
#include "Level.h"
#include "PlayerAnimation.h"
#include "PlayerWeapons.h"

// The Player class now delegates collision detection to a separate CollisionHandler (see CollisionHandler.h)
// and relies on ResourceManager (used in its components) for texture management.
class Player
{
public:
    Player(SDL_Renderer *renderer, Level *level);
    ~Player();

    void updateInput(const Uint8 *keys);
    void shoot(int mouseX, int mouseY, int cameraX, int cameraY);
    void update(int screenWidth, int screenHeight);
    void render(SDL_Renderer *renderer, int cameraX, int cameraY);

    float getX() const { return posX; }
    float getY() const { return posY; }
    float getWidth() const { return spriteWidth; }
    float getHeight() const { return spriteHeight; }
    float getAngle() const { return angle; }
    void setAngle(float newAngle) { angle = newAngle; }

    // Exposed movement variables.
    float velX, velY;
    bool isMoving;

    std::vector<Bullet> &getBullets() { return bullets; }

    int getHealth() const { return health; }
    bool isDead() const { return dead; }
    void takeDamage(int dmg)
    {
        health -= dmg;
        if (health <= 0)
        {
            health = 0;
            dead = true;
        }
    }

    PlayerWeapons *getWeapons() { return weapons.get(); }
    PlayerAnimation *getAnimation() { return animation.get(); }

private:
    SDL_Renderer *renderer; // Not owned.
    Level *level;           // Not owned.

    std::unique_ptr<PlayerAnimation> animation;
    std::unique_ptr<PlayerWeapons> weapons;

    float posX, posY;
    float angle = 0.0f;
    const float speed = 3.0f;
    const int spriteWidth = 54, spriteHeight = 54;

    SDL_Rect collisionBox;
    std::vector<Bullet> bullets;
    bool bareFistAttacking = false;

    int health;
    bool dead;

    // Removed direct collision detection; see CollisionHandler.
};

#endif // PLAYER_H
