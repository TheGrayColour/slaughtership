// Bullet.h
#ifndef BULLET_H
#define BULLET_H

#include <SDL2/SDL.h>

/**
 * @brief Represents a projectile fired in the game.
 *
 * The Bullet class manages its position, movement (with delta time for frame-rate independence),
 * and rendering. The "active" flag indicates whether the bullet is still in play.
 * A bullet becomes inactive when it goes off-screen or, in the future, when it hits a target.
 *
 * Potential improvements:
 * - Extend this class with inheritance or components to include properties such as damage or lifespan.
 */
class Bullet
{
public:
    /**
     * @brief Constructs a Bullet object.
     *
     * @param x Initial x position.
     * @param y Initial y position.
     * @param dx Normalized direction along the x-axis.
     * @param dy Normalized direction along the y-axis.
     * @param speed Speed of the bullet.
     */
    Bullet(float x, float y, float dx, float dy, float speed);

    /**
     * @brief Updates the bullet's position based on delta time.
     *
     * This method adjusts the bullet's position by incorporating a delta time parameter (dt),
     * ensuring frame-rate independent movement.
     *
     * @param dt Delta time since the last update (in seconds).
     * @param screenWidth Width of the screen (used to determine off-screen status).
     * @param screenHeight Height of the screen (used to determine off-screen status).
     */
    void update(int screenWidth, int screenHeight);

    /**
     * @brief Renders the bullet on the screen.
     *
     * @param renderer SDL renderer.
     * @param cameraX Camera x offset.
     * @param cameraY Camera y offset.
     */
    void render(SDL_Renderer *renderer, int cameraX, int cameraY);

    /**
     * @brief Indicates whether the bullet is active.
     *
     * A bullet is active as long as it is on-screen (or has not collided with a target).
     * It becomes inactive if it moves off-screen.
     *
     * @return true if the bullet is active, false otherwise.
     */
    bool isActive() const { return active; }

private:
    float x, y;
    float dx, dy; // Normalized direction vector.
    float speed;
    bool active; // True if bullet is in play; false if it has left the screen or collided.
};

#endif // BULLET_H
