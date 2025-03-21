#include "PlayerWeapons.h"
#include <utility>

PlayerWeapons::PlayerWeapons()
{
}

PlayerWeapons::~PlayerWeapons()
{
    // Unique pointer auto-cleans.
}

void PlayerWeapons::pickupWeapon(Weapon weapon)
{
    // If there is no weapon held, take the first from dropped weapons.
    if (!currentWeapon)
    {
        currentWeapon = std::make_unique<Weapon>(std::move(weapon));
    }
}

void PlayerWeapons::dropWeapon(float playerX, float playerY)
{
    if (currentWeapon)
    {
        currentWeapon->setPosition(playerX, playerY);
        droppedWeapons.push_back(std::move(*currentWeapon));
        currentWeapon.reset();
    }
}

void PlayerWeapons::update()
{
    if (currentWeapon)
        currentWeapon->update();
}

void PlayerWeapons::shoot(std::vector<Bullet> &bullets, float playerX, float playerY, float aimX, float aimY)
{
    if (currentWeapon)
    {
        if (currentWeapon->isMelee())
            currentWeapon->attack();
        else
            currentWeapon->shoot(bullets, playerX, playerY, aimX, aimY);
    }
}

void PlayerWeapons::render(SDL_Renderer *renderer, float cameraX, float cameraY)
{
    if (currentWeapon)
    {
        // Render the weapon held by the player.
        SDL_Rect weaponSrc = {0, 0, 54, 54}; // Example values; adjust as needed.
        SDL_Rect destRect = {0, 0, 54, 54};  // These would be calculated based on player's position.
        // For example, one might integrate it into Player::render().
        SDL_RenderCopy(renderer, currentWeapon->getHeldTexture(), &weaponSrc, &destRect);
    }
    // Render dropped weapons.
    for (auto &weapon : droppedWeapons)
    {
        weapon.render(renderer, weapon.getX() - cameraX, weapon.getY() - cameraY);
    }
}

bool PlayerWeapons::hasWeapon() const
{
    return (currentWeapon != nullptr);
}
