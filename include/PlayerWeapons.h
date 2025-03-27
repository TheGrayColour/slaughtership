#ifndef PLAYER_WEAPONS_H
#define PLAYER_WEAPONS_H

#include "Weapon.h" // Now defines AbstractWeapon, ProjectileWeapon, MeleeWeapon
#include "Bullet.h"
#include <vector>
#include <memory>

class PlayerWeapons
{
public:
    PlayerWeapons();
    ~PlayerWeapons();

    // Pickup a new weapon. The argument can be used to decide which derived class to instantiate.
    void pickupWeapon(std::unique_ptr<AbstractWeapon> newWeapon, SDL_Renderer *renderer);

    // Drop the current weapon.
    std::unique_ptr<AbstractWeapon> dropWeapon(float playerX, float playerY);

    // Update the current weapon (e.g., update its animations or cooldowns).
    void update();

    // Have the current weapon shoot.
    void shoot(std::vector<Bullet> &bullets, float playerX, float playerY, float aimX, float aimY);

    // Render the current weapon (if any) and any dropped weapons.
    void render(SDL_Renderer *renderer, float playerX, float playerY, float angle);

    // Accessor to check if a weapon is currently held.
    bool hasWeapon() const;

    bool isMeleeWeapon() const { return currentWeapon && currentWeapon->isMelee(); }
    bool isAttacking() const;

    std::unique_ptr<AbstractWeapon> releaseCurrentWeapon();

private:
    // Currently held weapon.
    std::unique_ptr<AbstractWeapon> currentWeapon;

    // Dropped weapons stored as unique pointers.
    std::vector<std::unique_ptr<AbstractWeapon>> droppedWeapons;

    float fireTimer = 0.0f;
};

#endif // PLAYER_WEAPONS_H
