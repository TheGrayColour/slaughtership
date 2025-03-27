#ifndef WEAPON_H
#define WEAPON_H

#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "Bullet.h"

// Enumeration for weapon types.
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

// Abstract base class for weapons.
class AbstractWeapon
{
public:
    virtual ~AbstractWeapon() = default;

    // Shoot: spawns bullets or triggers a melee attack.
    virtual void shoot(std::vector<Bullet> &bullets, float playerX, float playerY, float aimX, float aimY) = 0;

    // Update weapon state (cooldowns, animations, etc.)
    virtual void update() = 0;

    // Render any weapon-specific effects (e.g. fire/attack animations).
    virtual void render(SDL_Renderer *renderer, float posX, float posY, float angle, bool dropped) = 0;

    // Initialize textures/resources.
    virtual void initialize(SDL_Renderer *renderer) = 0;

    // Check for ammo (for projectile weapons).
    virtual bool hasAmmo() const = 0;

    // Returns true if this is a melee weapon.
    virtual bool isMelee() const = 0;

    // Get the weapon type.
    virtual WeaponType getType() const = 0;

    virtual float getX() const = 0;
    virtual float getY() const = 0;
    virtual void setPosition(float newX, float newY) = 0;

    virtual float getFireRate() const = 0;
};

// Projectile weapon implementation.
class ProjectileWeapon : public AbstractWeapon
{
public:
    ProjectileWeapon(WeaponType type, int ammo, float fireRate, float bulletSpeed, int damage);
    virtual ~ProjectileWeapon() = default;

    virtual void shoot(std::vector<Bullet> &bullets, float playerX, float playerY, float aimX, float aimY) override;
    virtual void update() override;
    virtual void render(SDL_Renderer *renderer, float playerX, float playerY, float angle, bool dropped) override;
    virtual void initialize(SDL_Renderer *renderer) override;

    virtual bool hasAmmo() const override;
    virtual bool isMelee() const override { return false; }
    virtual WeaponType getType() const override { return type; }

    virtual float getFireRate() const override { return fireRate; }

    int getDamage() const { return damage; }

    float getX() const { return x; }
    float getY() const { return y; }
    void setPosition(float newX, float newY)
    {
        x = newX;
        y = newY;
    }

private:
    WeaponType type;
    int ammo;       // -1 for unlimited.
    float fireRate; // Time between shots.
    float bulletSpeed;
    int damage;

    float x, y; // Position of the weapon when dropped

    // Firing animation state.
    bool isFiring;
    int fireFrame;
    int fireFrameTime;
    const int FIRE_FRAMES = 4;
    const int FIRE_FRAME_SPEED = 3;

    // Texture pointers (managed via ResourceManager).
    SDL_Texture *heldTexture;
    SDL_Texture *droppedTexture;
    SDL_Texture *fireTexture;
};

// Melee weapon implementation.
class MeleeWeapon : public AbstractWeapon
{
public:
    MeleeWeapon(WeaponType type, float fireRate, int damage);
    virtual ~MeleeWeapon() = default;

    virtual void shoot(std::vector<Bullet> &bullets, float playerX, float playerY, float aimX, float aimY) override;
    virtual void update() override;
    virtual void render(SDL_Renderer *renderer, float playerX, float playerY, float angle, bool dropped) override;
    virtual void initialize(SDL_Renderer *renderer) override;

    virtual bool hasAmmo() const override { return true; } // Unlimited for melee.
    virtual bool isMelee() const override { return true; }
    virtual WeaponType getType() const override { return type; }

    int getDamage() const { return damage; }

    virtual float getX() const override { return x; }
    virtual float getY() const override { return y; }
    virtual void setPosition(float newX, float newY) override
    {
        x = newX;
        y = newY;
    }

private:
    WeaponType type;
    float fireRate;
    int damage;

    float x, y; // Position when dropped

    // Attack animation state.
    bool isAttacking;
    int attackFrame;
    int attackFrameTime;
    const int ATTACK_FRAMES = 8;
    const int ATTACK_FRAME_SPEED = 3;

    // Texture pointers.
    SDL_Texture *heldTexture;
    SDL_Texture *attackTexture;
    SDL_Texture *droppedTexture;
};

#endif // WEAPON_H
