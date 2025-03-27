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

std::unique_ptr<AbstractWeapon> PlayerWeapons::dropWeapon(float playerX, float playerY)
{
    if (currentWeapon)
    {
        currentWeapon->setPosition(playerX, playerY);
        // Release the current weapon to the caller.
        return std::move(currentWeapon);
    }
    return nullptr;
}

void PlayerWeapons::render(SDL_Renderer *renderer, float playerX, float playerY, float angle)
{
    if (currentWeapon)
    {
        currentWeapon->render(renderer, playerX, playerY, angle, false);
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

std::unique_ptr<AbstractWeapon> PlayerWeapons::releaseCurrentWeapon()
{
    // This returns the current weapon and sets currentWeapon to nullptr.
    return std::move(currentWeapon);
}

bool PlayerWeapons::isAttacking() const
{
    // Only melee weapons have an attack state.
    if (currentWeapon && currentWeapon->isMelee())
    {
        // For melee weapons, downcast and check:
        const MeleeWeapon *mw = dynamic_cast<const MeleeWeapon *>(currentWeapon.get());
        if (mw)
            return mw->isCurrentlyAttacking();
    }
    return false;
}
