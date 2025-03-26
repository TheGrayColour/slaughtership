#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h>
#include <vector>
#include "Constants.h"
#include "CollisionHandler.h"
#include "Weapon.h"

enum class EnemyState
{
    IDLE,
    PATROLLING,
    ATTACKING,
    DEAD
};

class Enemy
{
public:
    // Construct an enemy at (x, y) with default parameters.
    Enemy(float x, float y, SDL_Renderer *renderer);

    // Update enemy behavior:
    //  - dt: Delta time (in seconds)
    //  - playerRect: Player's on-screen rectangle (for detection)
    //  - walls: Collision boundaries for patrolling
    void update(float dt, const SDL_Rect &playerRect, const std::vector<SDL_Rect> &walls, std::vector<Bullet> &enemyBullets, bool playerAlive);

    // Render the enemy (alive or dead) with its current animation and facing angle.
    // cameraX/Y are world-to-screen offsets.
    void render(SDL_Renderer *renderer, int cameraX, int cameraY);

    // Inflict damage; if health drops to or below zero, enemy dies.
    void takeDamage(int damage);

    // Returns true if enemy is dead.
    bool isDead() const { return state == EnemyState::DEAD; }

    // Get enemy position.
    float getX() const { return x; }
    float getY() const { return y; }

    SDL_Rect getCollisionBox() const { return collisionBox; }

    void attack(std::vector<Bullet> &enemyBullets, const SDL_Rect &playerRect);
    std::unique_ptr<AbstractWeapon> dropWeapon();

private:
    float x, y; // World position of the enemy.
    SDL_Rect collisionBox;
    float speed;      // Movement speed.
    int health;       // Health points.
    EnemyState state; // Current state.
    float angle;      // Facing angle (in degrees).

    // Textures for alive and dead states.
    SDL_Texture *enemyIdleTexture;
    SDL_Texture *enemyRunTexture;
    SDL_Texture *deadTexture;

    int runFrame = 0;
    int runFrameTime = 0;
    const int RUN_FRAMES = 8;
    const int RUN_FRAME_SPEED = 3; // Adjust as needed

    int deathFrame;
    int deathFrameTime;
    const int DEATH_FRAMES = 8;
    const int DEATH_FRAME_SPEED = 3;
    bool deathAnimationPlayed;

    // Simple AI methods.
    void patrol(float dt, const std::vector<SDL_Rect> &walls);
    void engagePlayer(const SDL_Rect &playerRect);

    std::unique_ptr<AbstractWeapon> weapon;

    float fireTimer = 0.0f;
};

#endif // ENEMY_H
