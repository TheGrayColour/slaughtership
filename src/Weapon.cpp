#include "Weapon.h"
#include "Constants.h"
#include "ResourceManager.h"
#include <SDL2/SDL_image.h>
#include <cmath>
#include <iostream>

// ----------------- ProjectileWeapon -----------------
ProjectileWeapon::ProjectileWeapon(WeaponType type, int ammo, float fireRate, float bulletSpeed, int damage)
    : type(type), ammo(ammo), fireRate(fireRate), bulletSpeed(bulletSpeed), damage(damage), x(0), y(0),
      isFiring(false), fireFrame(0), fireFrameTime(0),
      heldTexture(nullptr), droppedTexture(nullptr), fireTexture(nullptr)
{
}

void ProjectileWeapon::initialize(SDL_Renderer *renderer)
{
    std::string basePath = "assets/weapons/";
    std::string weaponName;
    switch (type)
    {
    case WeaponType::PISTOL:
        weaponName = "pistol";
        break;
    case WeaponType::SHOTGUN:
        weaponName = "shotgun";
        break;
    case WeaponType::SMG:
        weaponName = "smg";
        break;
    case WeaponType::AK:
        weaponName = "ak";
        break;
    case WeaponType::MG:
        weaponName = "mg";
        break;
    case WeaponType::UZI:
        weaponName = "uzi";
        break;
    default:
        weaponName = "default";
        break;
    }
    heldTexture = ResourceManager::loadTexture(renderer, basePath + weaponName + "_held.png");
    droppedTexture = ResourceManager::loadTexture(renderer, basePath + weaponName + "_dropped.png");
    fireTexture = ResourceManager::loadTexture(renderer, basePath + "fire.png");
}

void ProjectileWeapon::shoot(std::vector<Bullet> &bullets, float playerX, float playerY, float aimX, float aimY)
{
    if (!hasAmmo())
        return;

    float dx = aimX - playerX;
    float dy = aimY - playerY;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len == 0)
        return;
    dx /= len;
    dy /= len;

    // Spawn a new bullet.
    bullets.emplace_back(playerX, playerY, dx, dy, bulletSpeed);
    if (ammo > 0)
        ammo--;

    isFiring = true;
    fireFrame = 0;
    fireFrameTime = 0;
}

void ProjectileWeapon::update()
{
    if (isFiring)
    {
        fireFrameTime++;
        if (fireFrameTime >= FIRE_FRAME_SPEED)
        {
            fireFrame++;
            fireFrameTime = 0;
        }
        if (fireFrame >= FIRE_FRAMES)
        {
            isFiring = false;
            fireFrame = 0;
        }
    }
}

void ProjectileWeapon::render(SDL_Renderer *renderer, float playerX, float playerY, float angle)
{
    // Render the held weapon using rotation:
    if (heldTexture)
    {
        SDL_Rect destRect;
        destRect.x = static_cast<int>(playerX + (PLAYER_SPRITE_WIDTH - 54) / 2);
        destRect.y = static_cast<int>(playerY + (PLAYER_SPRITE_HEIGHT - 54) / 2);
        destRect.w = 54;
        destRect.h = 54;
        SDL_Point center = {27, 27}; // center of 54x54
        SDL_RenderCopyEx(renderer, heldTexture, nullptr, &destRect, angle, &center, SDL_FLIP_NONE);
    }

    // Render the fire animation at the tip of the weapon:
    if (isFiring && fireTexture)
    {
        SDL_Rect fireSrc = {fireFrame * 16, 0, 16, 16};
        // Define an offset from the player's center to where the weapon tip is located.
        const float offset = 25.0f; // adjust this value as needed.
        float offsetX = offset * cos(angle * M_PI / 180.0f);
        float offsetY = offset * sin(angle * M_PI / 180.0f);
        SDL_Rect fireDest;
        // Position the fire effect at player's center plus offset, adjusting for fire texture size:
        fireDest.x = static_cast<int>(playerX + PLAYER_SPRITE_WIDTH / 2 + offsetX - 8);
        fireDest.y = static_cast<int>(playerY + PLAYER_SPRITE_HEIGHT / 2 + offsetY - 8);
        fireDest.w = 16;
        fireDest.h = 16;
        SDL_Point fireCenter = {8, 8};
        SDL_RenderCopyEx(renderer, fireTexture, &fireSrc, &fireDest, angle, &fireCenter, SDL_FLIP_NONE);
    }
}

bool ProjectileWeapon::hasAmmo() const
{
    return (ammo > 0 || ammo == -1);
}

// ----------------- MeleeWeapon -----------------
MeleeWeapon::MeleeWeapon(WeaponType type, float fireRate, int damage)
    : type(type), fireRate(fireRate), damage(damage), x(0), y(0),
      isAttacking(false), attackFrame(0), attackFrameTime(0),
      heldTexture(nullptr), attackTexture(nullptr)
{
}

void MeleeWeapon::initialize(SDL_Renderer *renderer)
{
    std::string basePath = "assets/weapons/";
    std::string weaponName;
    switch (type)
    {
    case WeaponType::BASEBALL_BAT:
        weaponName = "bat";
        break;
    case WeaponType::KNIFE:
        weaponName = "knife";
        break;
    case WeaponType::BAREFIST:
        weaponName = "barefist";
        break;
    default:
        weaponName = "default";
        break;
    }
    heldTexture = ResourceManager::loadTexture(renderer, basePath + weaponName + "_held.png");
    attackTexture = ResourceManager::loadTexture(renderer, basePath + weaponName + "_attack.png");
}

void MeleeWeapon::shoot(std::vector<Bullet> & /*bullets*/, float playerX, float playerY, float /*aimX*/, float /*aimY*/)
{
    // For melee, shooting triggers an attack.
    if (!isAttacking)
    {
        isAttacking = true;
        attackFrame = 0;
        attackFrameTime = 0;
    }
}

void MeleeWeapon::update()
{
    if (isAttacking)
    {
        attackFrameTime++;
        if (attackFrameTime >= ATTACK_FRAME_SPEED)
        {
            attackFrame++;
            attackFrameTime = 0;
        }
        if (attackFrame >= ATTACK_FRAMES)
        {
            isAttacking = false;
            attackFrame = 0;
        }
    }
}

void MeleeWeapon::render(SDL_Renderer *renderer, float playerX, float playerY, float angle)
{
    // Render the held (static) weapon image.
    if (heldTexture)
    {
        SDL_Rect destRect;
        // Center the held texture over the player.
        destRect.x = static_cast<int>(playerX + (PLAYER_SPRITE_WIDTH - 54) / 2);
        destRect.y = static_cast<int>(playerY + (PLAYER_SPRITE_HEIGHT - 54) / 2);
        destRect.w = 54;
        destRect.h = 54;
        SDL_Point center = {27, 27}; // center of a 54x54 texture.
        SDL_RenderCopyEx(renderer, heldTexture, nullptr, &destRect, angle, &center, SDL_FLIP_NONE);
    }

    // If attacking, render the attack animation over the held weapon.
    if (isAttacking && attackTexture)
    {
        SDL_Rect attackSrc = {attackFrame * 54, 0, 54, 54};
        SDL_Rect attackDest;
        attackDest.x = static_cast<int>(playerX + (PLAYER_SPRITE_WIDTH - 54) / 2);
        attackDest.y = static_cast<int>(playerY + (PLAYER_SPRITE_HEIGHT - 54) / 2);
        attackDest.w = 54;
        attackDest.h = 54;
        SDL_Point center = {27, 27};
        SDL_RenderCopyEx(renderer, attackTexture, &attackSrc, &attackDest, angle, &center, SDL_FLIP_NONE);
    }
}
