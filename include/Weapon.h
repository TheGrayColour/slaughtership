#ifndef WEAPON_H
#define WEAPON_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include "Bullet.h"

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

    void shoot(std::vector<Bullet> &bullets, float playerX, float playerY, float aimX, float aimY);
    void render(SDL_Renderer *renderer, float playerX, float playerY);
    void renderAttack(SDL_Renderer *renderer, float playerX, float playerY);
    void renderFire(SDL_Renderer *renderer, float playerX, float playerY);
    void attack(); // New function for melee attack animations
    void update(); // Handles attack animation timing
    void initialize(SDL_Renderer *renderer);

    float getX() const { return x; }
    float getY() const { return y; }

    void setPosition(float newX, float newY)
    {
        x = newX;
        y = newY;
    }
    bool hasAmmo() const { return ammo > 0 || ammo == -1; } // -1 for melee

    SDL_Texture *getDroppedTexture() const { return droppedTexture; }
    SDL_Texture *getHeldTexture();

    bool isMelee() const { return type == WeaponType::BAREFIST || type == WeaponType::BASEBALL_BAT || type == WeaponType::KNIFE; }
    bool isCurrentlyAttacking() const { return isAttacking; }

private:
    WeaponType type;
    int ammo;
    float x, y;
    float fireRate, bulletSpeed;

    // Textures for weapon states
    SDL_Texture *heldTexture;    // For when the weapon is equipped
    SDL_Texture *droppedTexture; // For when the weapon is dropped
    SDL_Texture *attackTexture;  // For melee attack animation

    bool isFiring;
    int fireFrame;
    int fireFrameTime;
    SDL_Texture *fireTexture;

    // Attack animation state
    bool isAttacking;
    int attackFrame;
    int attackFrameTime;

    int ATTACK_FRAMES = 8;      // Number of attack frames (bat/knife)
    int ATTACK_FRAME_SPEED = 3; // Speed of attack animation
    int FIRE_FRAMES = 4;        // Number of attack frames (bat/knife)
    int FIRE_FRAME_SPEED = 3;   // Speed of attack animation
};

#endif
