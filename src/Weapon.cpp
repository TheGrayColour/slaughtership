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

void ProjectileWeapon::render(SDL_Renderer *renderer, float posX, float posY, float angle, bool dropped)
{
    // Destination rectangle for rendering (assume 54x54 size)
    SDL_Rect destRect = {0, 0, 54, 54};
    // If held, center it over the owner (player or enemy)
    if (!dropped)
    {
        destRect.x = static_cast<int>(posX + (PLAYER_SPRITE_WIDTH - 54) / 2);
        destRect.y = static_cast<int>(posY + (PLAYER_SPRITE_HEIGHT - 54) / 2);
        SDL_Point center = {27, 27};
        SDL_RenderCopyEx(renderer, heldTexture, nullptr, &destRect, angle, &center, SDL_FLIP_NONE);

        // Render fire animation if firing.
        if (isFiring && fireTexture)
        {
            SDL_Rect fireSrc = {fireFrame * 16, 0, 16, 16};
            const float offset = 25.0f;
            float rad = angle * M_PI / 180.0f;
            float offsetX = offset * cos(rad);
            float offsetY = offset * sin(rad);
            SDL_Rect fireDest;
            fireDest.x = static_cast<int>(posX + PLAYER_SPRITE_WIDTH / 2 + offsetX - 8);
            fireDest.y = static_cast<int>(posY + PLAYER_SPRITE_HEIGHT / 2 + offsetY - 8);
            fireDest.w = 16;
            fireDest.h = 16;
            SDL_Point fireCenter = {8, 8};
            SDL_RenderCopyEx(renderer, fireTexture, &fireSrc, &fireDest, angle, &fireCenter, SDL_FLIP_NONE);
        }
    }
    else
    {
        int texW = 0, texH = 0;
        SDL_QueryTexture(droppedTexture, NULL, NULL, &texW, &texH);
        // Optionally, scale the texture (for example, half size):
        float scale = 1.0f; // adjust as needed
        SDL_Rect destRect;
        destRect.x = static_cast<int>(posX);
        destRect.y = static_cast<int>(posY);
        destRect.w = static_cast<int>(texW * scale);
        destRect.h = static_cast<int>(texH * scale);
        SDL_RenderCopy(renderer, droppedTexture, nullptr, &destRect);
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
      heldTexture(nullptr), attackTexture(nullptr), droppedTexture(nullptr)
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
    droppedTexture = ResourceManager::loadTexture(renderer, basePath + weaponName + "_dropped.png");
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

void MeleeWeapon::render(SDL_Renderer *renderer, float posX, float posY, float angle, bool dropped)
{
    SDL_Rect destRect = {0, 0, 54, 54};

    if (!dropped)
    {
        // Held weapon: center it relative to the owner.
        destRect.x = static_cast<int>(posX + (PLAYER_SPRITE_WIDTH - 54) / 2);
        destRect.y = static_cast<int>(posY + (PLAYER_SPRITE_HEIGHT - 54) / 2);
        destRect.w = 54;
        destRect.h = 54;
        SDL_Point center = {27, 27};
        if (isAttacking && attackTexture)
        {
            // During attack, render the attack animation only.
            SDL_Rect srcRect = {attackFrame * 54, 0, 54, 54};
            SDL_RenderCopyEx(renderer, attackTexture, &srcRect, &destRect, angle, &center, SDL_FLIP_NONE);
        }
        else
        {
            // Otherwise, render the held texture.
            SDL_RenderCopyEx(renderer, heldTexture, nullptr, &destRect, angle, &center, SDL_FLIP_NONE);
        }
    }
    else
    {
        // When dropped, render the dropped image.
        int texW = 0, texH = 0;
        SDL_QueryTexture(droppedTexture, NULL, NULL, &texW, &texH);
        // Optionally, scale the texture (for example, half size):
        float scale = 1.0f; // adjust as needed
        SDL_Rect destRect;
        destRect.x = static_cast<int>(posX);
        destRect.y = static_cast<int>(posY);
        destRect.w = static_cast<int>(texW * scale);
        destRect.h = static_cast<int>(texH * scale);
        SDL_RenderCopy(renderer, droppedTexture, nullptr, &destRect);
    }
}
