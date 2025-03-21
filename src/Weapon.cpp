#include "Weapon.h"
#include "Constants.h"
#include <SDL2/SDL_image.h>
#include <cmath>
#include <iostream>

Weapon::Weapon(WeaponType type, float x, float y)
    : type(type), x(x), y(y), heldTexture(nullptr), droppedTexture(nullptr),
      attackTexture(nullptr),
      isFiring(false), fireFrame(0), fireFrameTime(0),
      isAttacking(false), attackFrame(0), attackFrameTime(0),
      ATTACK_FRAMES(8), ATTACK_FRAME_SPEED(3), FIRE_FRAMES(4), FIRE_FRAME_SPEED(3)
{
    // Set weapon properties based on type using constants.
    switch (type)
    {
    case WeaponType::PISTOL:
        ammo = WEAPON_AMMO_PISTOL;
        fireRate = WEAPON_FIRE_RATE_PISTOL;
        bulletSpeed = WEAPON_BULLET_SPEED_PISTOL;
        break;
    case WeaponType::SHOTGUN:
        ammo = WEAPON_AMMO_SHOTGUN;
        fireRate = WEAPON_FIRE_RATE_SHOTGUN;
        bulletSpeed = WEAPON_BULLET_SPEED_SHOTGUN;
        break;
    case WeaponType::BAREFIST:
    case WeaponType::BASEBALL_BAT:
    case WeaponType::KNIFE:
        ammo = WEAPON_AMMO_MELEE;
        fireRate = WEAPON_FIRE_RATE_MELEE;
        bulletSpeed = WEAPON_BULLET_SPEED_MELEE;
        break;
    default:
        ammo = WEAPON_AMMO_DEFAULT;
        fireRate = WEAPON_FIRE_RATE_DEFAULT;
        bulletSpeed = WEAPON_BULLET_SPEED_DEFAULT;
        break;
    }
}

void Weapon::initialize(SDL_Renderer *renderer)
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

    // Load held texture.
    SDL_Surface *heldSurface = IMG_Load(heldPath.c_str());
    if (!heldSurface)
    {
        std::cerr << "Failed to load held texture: " << heldPath << " SDL_Error: " << SDL_GetError() << std::endl;
    }
    else
    {
        SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, heldSurface);
        heldTexture.reset(tex);
        SDL_FreeSurface(heldSurface);
    }

    // Load dropped texture.
    SDL_Surface *droppedSurface = IMG_Load(droppedPath.c_str());
    if (!droppedSurface)
    {
        std::cerr << "Failed to load dropped texture: " << droppedPath << " SDL_Error: " << SDL_GetError() << std::endl;
    }
    else
    {
        SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, droppedSurface);
        droppedTexture.reset(tex);
        SDL_FreeSurface(droppedSurface);
    }

    // Load attack animation texture for melee weapons.
    if (type == WeaponType::BASEBALL_BAT || type == WeaponType::KNIFE || type == WeaponType::BAREFIST)
    {
        SDL_Surface *attackSurface = IMG_Load(attackPath.c_str());
        if (!attackSurface)
        {
            std::cerr << "Failed to load attack texture: " << attackPath << " SDL_Error: " << SDL_GetError() << std::endl;
        }
        else
        {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, attackSurface);
            attackTexture.reset(tex);
            SDL_FreeSurface(attackSurface);
        }
    }

    std::string firePath = basePath + weaponName + "_fire.png";
    SDL_Surface *fireSurface = IMG_Load(firePath.c_str());
    if (!fireSurface)
    {
        std::cerr << "Failed to load fire texture: " << firePath << " SDL_Error: " << SDL_GetError() << std::endl;
    }
    else
    {
        SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, fireSurface);
        fireTexture.reset(tex);
        SDL_FreeSurface(fireSurface);
    }
}

void Weapon::shoot(std::vector<Bullet> &bullets, float playerX, float playerY, float aimX, float aimY)
{
    if (ammo == 0)
        return; // No ammo available

    float dx = aimX - playerX;
    float dy = aimY - playerY;
    float length = std::sqrt(dx * dx + dy * dy);
    if (length == 0)
        return;
    dx /= length;
    dy /= length;

    bullets.emplace_back(playerX, playerY, dx, dy, bulletSpeed);
    if (ammo > 0)
        ammo--;

    // Trigger fire animation.
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
        if (fireFrame >= FIRE_FRAMES)
        {
            isFiring = false;
            fireFrame = 0;
        }
    }

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

SDL_Texture *Weapon::getHeldTexture()
{
    // If currently attacking and an attack texture exists, return it.
    if (isAttacking && attackTexture)
        return attackTexture.get();
    return heldTexture.get();
}

void Weapon::render(SDL_Renderer *renderer, float playerX, float playerY)
{
    if (!droppedTexture)
        return;
    // Render the dropped weapon as a small texture.
    SDL_Rect destRect = {static_cast<int>(x), static_cast<int>(y), 32, 32};
    SDL_RenderCopy(renderer, droppedTexture.get(), nullptr, &destRect);
}

void Weapon::renderAttack(SDL_Renderer *renderer, float playerX, float playerY)
{
    if (!isAttacking || !attackTexture)
        return;
    SDL_Rect attackSrc = {attackFrame * 54, 0, 54, 54}; // Assuming 8 frames of 54x54 pixels.
    SDL_Rect attackDest = {static_cast<int>(playerX), static_cast<int>(playerY), 54, 54};
    SDL_RenderCopy(renderer, attackTexture.get(), &attackSrc, &attackDest);
}

void Weapon::renderFire(SDL_Renderer *renderer, float playerX, float playerY)
{
    if (!isFiring || !fireTexture)
        return;
    SDL_Rect fireSrc = {fireFrame * 16, 0, 16, 16}; // Assuming 4 frames.
    SDL_Rect fireDest = {static_cast<int>(playerX) + 20, static_cast<int>(playerY) + 20, 16, 16};
    SDL_RenderCopy(renderer, fireTexture.get(), &fireSrc, &fireDest);
}
