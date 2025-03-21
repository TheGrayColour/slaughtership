#ifndef WEAPON_H
#define WEAPON_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <memory>
#include "Bullet.h"
#include "SDLDeleters.h" // For SDLTextureDeleter

enum class WeaponType
{
    BAREFIST,
    PISTOL,
    SHOTGUN,
    SMG,
    AK,
    MG,
    BASEBALL_BAT,
    KNIFE,
    UZI
};

class Weapon
{
public:
    Weapon(WeaponType type, float x = 0, float y = 0);

    // Disable copy constructor and copy assignment operator.
    Weapon(const Weapon &) = delete;
    Weapon &operator=(const Weapon &) = delete;

    // Allow move constructor and move assignment operator.
    Weapon(Weapon &&) = default;
    Weapon &operator=(Weapon &&) = default;

    void shoot(std::vector<Bullet> &bullets, float playerX, float playerY, float aimX, float aimY);
    void render(SDL_Renderer *renderer, float playerX, float playerY);
    void renderAttack(SDL_Renderer *renderer, float playerX, float playerY);
    void renderFire(SDL_Renderer *renderer, float playerX, float playerY);
    void attack(); // For melee attack animations
    void update(); // Handles animation timing
    void initialize(SDL_Renderer *renderer);

    float getX() const { return x; }
    float getY() const { return y; }

    void setPosition(float newX, float newY)
    {
        x = newX;
        y = newY;
    }
    bool hasAmmo() const { return ammo > 0 || ammo == -1; } // -1 means unlimited (melee)

    // Return the texture for a dropped weapon (read-only)
    SDL_Texture *getDroppedTexture() const { return droppedTexture.get(); }
    SDL_Texture *getHeldTexture();

    bool isMelee() const { return type == WeaponType::BAREFIST || type == WeaponType::BASEBALL_BAT || type == WeaponType::KNIFE; }
    bool isCurrentlyAttacking() const { return isAttacking; }

private:
    WeaponType type;
    int ammo;
    float x, y;
    float fireRate, bulletSpeed;

    // Textures for different weapon states wrapped in unique_ptr for automatic cleanup.
    std::unique_ptr<SDL_Texture, SDLTextureDeleter> heldTexture;    // When equipped
    std::unique_ptr<SDL_Texture, SDLTextureDeleter> droppedTexture; // When dropped
    std::unique_ptr<SDL_Texture, SDLTextureDeleter> attackTexture;  // Melee attack animation
    std::unique_ptr<SDL_Texture, SDLTextureDeleter> fireTexture;    // Fire animation

    bool isFiring;
    int fireFrame;
    int fireFrameTime;

    // Attack animation state
    bool isAttacking;
    int attackFrame;
    int attackFrameTime;

    int ATTACK_FRAMES = 8;      // Number of attack frames (bat/knife)
    int ATTACK_FRAME_SPEED = 3; // Speed of attack animation
    int FIRE_FRAMES = 4;        // Number of fire frames
    int FIRE_FRAME_SPEED = 3;   // Speed of fire animation
};

#endif
