#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "Bullet.h"
#include "Level.h"
#include "PlayerAnimation.h"
#include "PlayerWeapons.h"

// The Player class now focuses on core movement, state, and delegates
// animation and weapon handling to separate components.
class Player
{
public:
    Player(SDL_Renderer *renderer, Level *level);
    ~Player();

    // Processes keyboard input via InputManager.
    void updateInput(const Uint8 *keys);

    // Handles shooting. Delegates to the weapons component.
    // If no weapon is held, triggers a bare-fist attack.
    void shoot(int mouseX, int mouseY, int cameraX, int cameraY);

    // Updates the player's position (with collision), animation, weapons, and bullets.
    void update(int screenWidth, int screenHeight);

    // Renders the player (via animation), weapons, and bullets.
    void render(SDL_Renderer *renderer, int cameraX, int cameraY);

    // Convenience methods for weapon management.
    void pickupWeapon(/* Weapon object if needed */) { /* forward to weapons->pickupWeapon() */ }
    void dropWeapon() { weapons->dropWeapon(posX, posY); }

    // Getters.
    float getX() const { return posX; }
    float getY() const { return posY; }
    float getWidth() const { return spriteWidth; }
    float getHeight() const { return spriteHeight; }
    float getAngle() const { return angle; }
    void setAngle(float newAngle) { angle = newAngle; }

    // Expose movement variables for input processing.
    float velX, velY;
    bool isMoving;

private:
    SDL_Renderer *renderer; // Not owned.
    Level *level;           // Not owned.

    // Components responsible for animation and weapon handling.
    std::unique_ptr<PlayerAnimation> animation;
    std::unique_ptr<PlayerWeapons> weapons;

    // Core player state.
    float posX, posY;
    float angle = 0.0f;
    const float speed = 3.0f;
    const int spriteWidth = 54, spriteHeight = 54;

    // Collision box and bullets.
    SDL_Rect collisionBox;
    std::vector<Bullet> bullets;

    // Flag for bare-fist attack (when no weapon is held).
    bool bareFistAttacking = false;

    // Checks collision against level collision tiles.
    bool checkCollision(float newX, float newY);
};

#endif // PLAYER_H
