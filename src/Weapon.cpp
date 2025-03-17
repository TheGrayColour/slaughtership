#include "Weapon.h"
#include <SDL2/SDL_image.h>
#include <cmath>
#include <iostream>

Weapon::Weapon(WeaponType type, float x, float y)
    : type(type), x(x), y(y), heldTexture(nullptr), droppedTexture(nullptr),
      attackTexture(nullptr), isAttacking(false), attackFrame(0), attackFrameTime(0),
      ATTACK_FRAMES(8), ATTACK_FRAME_SPEED(3), FIRE_FRAMES(4), FIRE_FRAME_SPEED(3)
{
    // Set properties for each weapon type
    switch (type)
    {
    case WeaponType::PISTOL:
        ammo = 10;
        fireRate = 0.5f;
        bulletSpeed = 12.0f;
        break;
    case WeaponType::SHOTGUN:
        ammo = 5;
        fireRate = 1.0f;
        bulletSpeed = 10.0f;
        break;
    case WeaponType::BAREFIST:
    case WeaponType::BASEBALL_BAT:
    case WeaponType::KNIFE:
        ammo = -1; // Unlimited for melee
        fireRate = 0.3f;
        bulletSpeed = 0.0f;
        break;
    default:
        ammo = 20;
        fireRate = 0.2f;
        bulletSpeed = 14.0f;
        break;
    }
}

void Weapon::initialize(SDL_Renderer *renderer)
{
    std::string basePath = "assets/weapons/";

    // File names follow the format: "pistol_held.png" and "pistol_dropped.png"
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
    case WeaponType::BASEBALL_BAT:
        weaponName = "bat";
        break;
    case WeaponType::KNIFE:
        weaponName = "knife";
        break;
    case WeaponType::UZI:
        weaponName = "uzi";
        break;
    default:
        return;
    }

    std::string heldPath = basePath + weaponName + "_held.png";
    std::string droppedPath = basePath + weaponName + "_dropped.png";
    std::string attackPath = basePath + weaponName + "_attack.png";

    // Load held texture
    SDL_Surface *heldSurface = IMG_Load(heldPath.c_str());
    if (!heldSurface)
    {
        //
    }
    else
    {
        heldTexture = SDL_CreateTextureFromSurface(renderer, heldSurface);
        SDL_FreeSurface(heldSurface);
    }

    // Load dropped texture
    SDL_Surface *droppedSurface = IMG_Load(droppedPath.c_str());
    if (!droppedSurface)
    {
        //
    }
    else
    {
        droppedTexture = SDL_CreateTextureFromSurface(renderer, droppedSurface);
        SDL_FreeSurface(droppedSurface);
    }

    // Load attack animation (only for melee weapons)
    if (type == WeaponType::BASEBALL_BAT || type == WeaponType::KNIFE || type == WeaponType::BAREFIST)
    {
        SDL_Surface *attackSurface = IMG_Load(attackPath.c_str());
        if (!attackSurface)
        {
            //
        }
        else
        {
            attackTexture = SDL_CreateTextureFromSurface(renderer, attackSurface);
            SDL_FreeSurface(attackSurface);
        }
    }

    std::string firePath = basePath + weaponName + "_fire.png";
    SDL_Surface *fireSurface = IMG_Load(firePath.c_str());
    if (!fireSurface)
    {
        //
    }
    else
    {
        fireTexture = SDL_CreateTextureFromSurface(renderer, fireSurface);
        SDL_FreeSurface(fireSurface);
    }
}

void Weapon::shoot(std::vector<Bullet> &bullets, float playerX, float playerY, float aimX, float aimY)
{
    if (ammo == 0)
        return; // No ammo

    float dx = aimX - playerX;
    float dy = aimY - playerY;
    float length = sqrt(dx * dx + dy * dy);

    if (length == 0)
        return;

    dx /= length;
    dy /= length;

    bullets.emplace_back(playerX, playerY, dx, dy, bulletSpeed);
    if (ammo > 0)
        ammo--;

    // Start fire animation
    isFiring = true;
    fireFrame = 0;
    fireFrameTime = 0;
}

void Weapon::attack()
{
    if (type == WeaponType::BASEBALL_BAT || type == WeaponType::KNIFE || type == WeaponType::BAREFIST)
    {
        isAttacking = true;
        attackFrame = 0;
        attackFrameTime = 0;
    }
}

void Weapon::update()
{
    if (isFiring)
    {
        fireFrameTime++;
        if (fireFrameTime >= FIRE_FRAME_SPEED)
        {
            fireFrame++;
            fireFrameTime = 0;
        }

        if (fireFrame >= FIRE_FRAMES) // Reset animation
        {
            isFiring = false;
            fireFrame = 0;
        }
    }

    if (isAttacking)
    {
        attackFrameTime++;
        if (attackFrameTime >= ATTACK_FRAME_SPEED) // Adjust attack animation speed
        {
            attackFrame++;
            attackFrameTime = 0;
        }

        if (attackFrame >= ATTACK_FRAMES) // Reset attack animation
        {
            isAttacking = false;
            attackFrame = 0;
        }
    }
}

SDL_Texture *Weapon::getHeldTexture()
{
    return (isAttacking && attackTexture) ? attackTexture : heldTexture;
}

void Weapon::render(SDL_Renderer *renderer, float playerX, float playerY)
{
    if (!droppedTexture)
        return;

    SDL_Rect destRect = {static_cast<int>(x), static_cast<int>(y), 32, 32}; // Dropped weapons are small
    SDL_RenderCopy(renderer, droppedTexture, nullptr, &destRect);
}

void Weapon::renderAttack(SDL_Renderer *renderer, float playerX, float playerY)
{
    if (!isAttacking || !attackTexture)
        return;

    SDL_Rect attackSrc = {attackFrame * 54, 0, 54, 54}; // 8 frames, 54x54px
    SDL_Rect attackDest = {(int)playerX, (int)playerY, 54, 54};
    SDL_RenderCopy(renderer, attackTexture, &attackSrc, &attackDest);
}

void Weapon::renderFire(SDL_Renderer *renderer, float playerX, float playerY)
{
    if (!isFiring || !fireTexture)
        return;

    SDL_Rect fireSrc = {fireFrame * 16, 0, 16, 16}; // 4 frames
    SDL_Rect fireDest = {(int)playerX + 20, (int)playerY + 20, 16, 16};
    SDL_RenderCopy(renderer, fireTexture, &fireSrc, &fireDest);
}