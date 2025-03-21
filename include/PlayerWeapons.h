#ifndef PLAYER_WEAPONS_H
#define PLAYER_WEAPONS_H

#include "Weapon.h"
#include <vector>
#include <memory>

class PlayerWeapons
{
public:
    PlayerWeapons();
    ~PlayerWeapons();

    // Pickup a new weapon.
    void pickupWeapon(Weapon weapon);

    // Drop the current weapon.
    void dropWeapon(float playerX, float playerY);

    // Update the current weapon (e.g., update its animations).
    void update();

    // Have the current weapon shoot.
    void shoot(std::vector<Bullet> &bullets, float playerX, float playerY, float aimX, float aimY);

    // Render the current weapon (if any) and any dropped weapons.
    void render(SDL_Renderer *renderer, float cameraX, float cameraY);

    // Accessor to check if a weapon is currently held.
    bool hasWeapon() const;

    bool isAttacking() const
    {
        return currentWeapon ? currentWeapon->isCurrentlyAttacking() : false;
    }

private:
    // For simplicity, we’ll assume a single weapon is held at a time.
    std::unique_ptr<Weapon> currentWeapon;
    std::vector<Weapon> droppedWeapons;
};

#endif // PLAYER_WEAPONS_H
