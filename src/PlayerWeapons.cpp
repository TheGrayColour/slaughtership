#include "PlayerWeapons.h"
#include <utility>

PlayerWeapons::PlayerWeapons()
{
}

PlayerWeapons::~PlayerWeapons()
{
    // Unique pointers auto-clean.
}

void PlayerWeapons::pickupWeapon(std::unique_ptr<AbstractWeapon> newWeapon, SDL_Renderer *renderer)
{
    // Initialize the weapon's textures using the provided renderer.
    newWeapon->initialize(renderer);
    if (!currentWeapon)
    {
        currentWeapon = std::move(newWeapon);
    }
    else
    {
        // Optionally drop current weapon at a default position (or current player's pos passed in separately)
        dropWeapon(0, 0); // You might adjust this.
        currentWeapon = std::move(newWeapon);
    }
}

void PlayerWeapons::dropWeapon(float playerX, float playerY)
{
    if (currentWeapon)
    {
        currentWeapon->setPosition(playerX, playerY);
        droppedWeapons.push_back(std::move(currentWeapon));
    }
}

void PlayerWeapons::render(SDL_Renderer *renderer, float playerX, float playerY, float angle)
{
    if (currentWeapon)
    {
        currentWeapon->render(renderer, playerX, playerY, angle);
    }
    for (auto &weapon : droppedWeapons)
    {
        // Render dropped weapons using their stored coordinates.
        weapon->render(renderer, weapon->getX(), weapon->getY(), 0.0f);
    }
}

void PlayerWeapons::update()
{
    if (currentWeapon)
        currentWeapon->update();

    fireTimer += 1.0f / 60.0f;
}

void PlayerWeapons::shoot(std::vector<Bullet> &bullets, float playerX, float playerY, float aimX, float aimY)
{
    if (currentWeapon)
    {
        if (fireTimer < currentWeapon->getFireRate())
            return;
        currentWeapon->shoot(bullets, playerX, playerY, aimX, aimY);
        fireTimer = 0.0f;
    }
}

bool PlayerWeapons::hasWeapon() const
{
    return (currentWeapon != nullptr);
}
