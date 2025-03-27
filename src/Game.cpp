// Game.cpp
#include "Game.h"
#include "SDL2/SDL_image.h"
#include <iostream>
#include "Constants.h"
#include <cmath>

Game::Game() : running(false), inMenu(true), camera{0, 0, SCREEN_WIDTH, SCREEN_HEIGHT} {}

Game::~Game()
{
    clean();
}

bool Game::createWindowAndRenderer(const char *title, int width, int height)
{
    SDL_Window *rawWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                             width, height, SDL_WINDOW_SHOWN);
    if (!rawWindow)
    {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return false;
    }
    window.reset(rawWindow);

    try
    {
        renderer = std::make_unique<Renderer>(window.get());
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Renderer creation error: " << e.what() << std::endl;
        return false;
    }

    return true;
}

bool Game::init(const char *title, int width, int height)
{
    // Clear any previous data.
    enemies.clear();
    enemyBullets.clear();
    droppedWeapons.clear();

    // (Optional) Clear the resource manager so textures are reloaded.
    ResourceManager::clear();

    if (!createWindowAndRenderer(title, width, height))
        return false;

    SDL_Renderer *sdlRenderer = renderer->getSDLRenderer();
    menu = std::make_unique<Menu>(sdlRenderer);
    level = std::make_unique<Level>(sdlRenderer, "assets/map/map.json");
    player = std::make_unique<Player>(sdlRenderer, level.get());

    enemyBullets.clear();
    spawnEnemies(sdlRenderer);

    running = true;
    return true;
}

void Game::processGameInput(SDL_Event &event)
{
    if (event.type == SDL_QUIT)
    {
        running = false;
    }
    // Process left-click for shooting.
    else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        player->shoot(mouseX, mouseY, camera.x, camera.y);
    }
    // Process right-click for pickup/throw.
    else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT)
    {
        // Determine player's center.
        SDL_Point playerCenter = {static_cast<int>(player->getX() + player->getWidth() / 2),
                                  static_cast<int>(player->getY() + player->getHeight() / 2)};

        if (player->getWeapons()->hasWeapon())
        {
            // Calculate throw position based on mouse.
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            float playerCenterX = player->getX() + player->getWidth() / 2;
            float playerCenterY = player->getY() + player->getHeight() / 2;
            float dx = static_cast<float>(mouseX + camera.x) - playerCenterX;
            float dy = static_cast<float>(mouseY + camera.y) - playerCenterY;
            float len = std::sqrt(dx * dx + dy * dy);
            if (len == 0)
                len = 1;
            float offset = 5.0f;
            int throwX = static_cast<int>(playerCenterX + (dx / len) * offset);
            int throwY = static_cast<int>(playerCenterY + (dy / len) * offset);

            // Drop the currently held weapon and add it to the global droppedWeapons.
            auto dropped = player->getWeapons()->dropWeapon(throwX, throwY);
            if (dropped)
            {
                // The dropped weapon is now set at the throw position.
                droppedWeapons.push_back(std::move(dropped));
            }
        }
        else
        {
            // No weapon held: attempt to pick up a nearby dropped weapon.
            SDL_Rect pickupRect = {playerCenter.x - 16, playerCenter.y - 16, 32, 32};
            for (auto it = droppedWeapons.begin(); it != droppedWeapons.end();)
            {
                if (*it)
                {
                    SDL_Rect weaponRect = {static_cast<int>((*it)->getX()),
                                           static_cast<int>((*it)->getY()),
                                           32, 32}; // Adjust pickup area as needed.
                    if (SDL_HasIntersection(&weaponRect, &pickupRect))
                    {
                        // Pickup the first weapon found.
                        player->getWeapons()->pickupWeapon(std::move(*it), renderer->getSDLRenderer());
                        it = droppedWeapons.erase(it);
                        break; // Exit loop: only one weapon should be picked up.
                    }
                    else
                    {
                        ++it;
                    }
                }
                else
                {
                    ++it;
                }
            }
        }
    }
    // Additional game-specific event processing can be added here.
}

void Game::handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {

        // If player is dead, check for restart key.
        if (player->isDead() && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r)
        {
            // Restart the game: reinitialize state.
            // You can call a restart method or simply reinitialize game objects.
            clean();
            init("slaughtership", SCREEN_WIDTH, SCREEN_HEIGHT);
            return;
        }

        if (inMenu)
        {
            menu->handleEvents(event, running, inMenu);
        }
        else
        {
            processGameInput(event);

            // Process keyboard input for movement using InputManager.
            const Uint8 *keys = SDL_GetKeyboardState(nullptr);
            player->updateInput(keys);

            if (!player->isDead())
            {
                // Update player rotation to face the mouse.
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                float playerCenterX = player->getX() + (player->getWidth() / 2);
                float playerCenterY = player->getY() + (player->getHeight() / 2);
                float dx = (mouseX + camera.x) - playerCenterX;
                float dy = (mouseY + camera.y) - playerCenterY;
                float targetAngle = atan2(dy, dx) * (180.0 / M_PI);
                player->setAngle(targetAngle);
            }
        }
    }
}

void Game::update()
{
    if (!inMenu)
    {
        // Basic camera smoothing: interpolate current camera position toward desired position.
        int desiredX = player->getX() - camera.w / 2;
        int desiredY = player->getY() - camera.h / 2;
        float smoothingFactor = 0.1f;
        camera.x = static_cast<int>(camera.x + smoothingFactor * (desiredX - camera.x));
        camera.y = static_cast<int>(camera.y + smoothingFactor * (desiredY - camera.y));

        // Update player state.
        player->update(SCREEN_WIDTH, SCREEN_HEIGHT);

        // Build player's collision rectangle.
        SDL_Rect playerRect = {static_cast<int>(player->getX()),
                               static_cast<int>(player->getY()),
                               static_cast<int>(player->getWidth()),
                               static_cast<int>(player->getHeight())};

        // Determine if a melee attack is occurring:
        bool meleeAttack = false;
        if (!player->getWeapons()->hasWeapon())
        {
            // No weapon: barefist attack based on player animation.
            meleeAttack = player->getAnimation()->isAttacking();
        }
        else if (player->getWeapons()->isMeleeWeapon() && player->getWeapons()->isAttacking())
        {
            // With melee weapon: check the weapon's attack state.
            meleeAttack = true;
        }

        if (!player->isDead() && meleeAttack)
        {
            // Define melee attack area: for simplicity, we use the player's collision box.
            SDL_Rect meleeArea = {static_cast<int>(player->getX()) + PLAYER_COLLISION_OFFSET_X,
                                  static_cast<int>(player->getY()) + PLAYER_COLLISION_OFFSET_Y,
                                  PLAYER_COLLISION_WIDTH, PLAYER_COLLISION_HEIGHT};
            for (auto &enemy : enemies)
            {
                SDL_Rect enemyBox = enemy->getCollisionBox();
                if (!enemy->isDead() && SDL_HasIntersection(&meleeArea, &enemyBox))
                {
                    enemy->takeDamage(9999); // Instantly kill enemy.
                }
            }
        }

        // Update each enemy.
        // Pass a fixed delta time (1/60 seconds), the player's rectangle, and level wall collisions.
        for (auto &enemy : enemies)
        {
            enemy->update(1.0f / 60.0f, playerRect, level->getCollisionTiles(), enemyBullets, !player->isDead());
        }

        // Drop weapons from dead enemies.
        for (auto &enemy : enemies)
        {
            if (enemy->isDead() && enemy->hasWeapon())
            {
                droppedWeapons.push_back(enemy->dropWeapon());
            }
        }

        // Update enemy bullets.
        for (auto &bullet : enemyBullets)
        {
            bullet.update(1.0f, SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        // Check enemy bullet collision with the player.
        // Use the player's collision box (using the same offsets as for the player).
        SDL_Rect playerCollision = {static_cast<int>(player->getX()) + PLAYER_COLLISION_OFFSET_X,
                                    static_cast<int>(player->getY()) + PLAYER_COLLISION_OFFSET_Y,
                                    PLAYER_COLLISION_WIDTH, PLAYER_COLLISION_HEIGHT};

        for (auto &bullet : enemyBullets)
        {
            SDL_Rect bulletRect = {static_cast<int>(bullet.getX()), static_cast<int>(bullet.getY()), 5, 5};

            // Check collision with walls:
            for (const auto &wall : level->getCollisionTiles())
            {
                if (SDL_HasIntersection(&bulletRect, &wall))
                {
                    bullet.deactivate();
                    break; // No need to check further walls for this bullet.
                }
            }

            if (SDL_HasIntersection(&playerCollision, &bulletRect))
            {
                player->takeDamage(9999);
                bullet.deactivate();
            }
        }

        auto &bullets = player->getBullets();
        SDL_Rect bulletRect;
        for (auto &bullet : bullets)
        {
            bulletRect.x = static_cast<int>(bullet.getX());
            bulletRect.y = static_cast<int>(bullet.getY());
            bulletRect.w = 5;
            bulletRect.h = 5;

            // Check collision with walls:
            for (const auto &wall : level->getCollisionTiles())
            {
                if (SDL_HasIntersection(&bulletRect, &wall))
                {
                    bullet.deactivate();
                    break; // No need to check further walls for this bullet.
                }
            }

            // Check collision with enemies:
            for (auto &enemy : enemies)
            {
                SDL_Rect enemyBox = enemy->getCollisionBox();
                if (!enemy->isDead() && SDL_HasIntersection(&bulletRect, &enemyBox))
                {
                    enemy->takeDamage(9999); // Instantly kill enemy.
                    bullet.deactivate();
                    break; // Exit loop since bullet is deactivated.
                }
            }
        }

        // Remove deactivated bullets.
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                     [](const Bullet &b)
                                     { return !b.isActive(); }),
                      bullets.end());

        enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(),
                                          [](const Bullet &b)
                                          { return !b.isActive(); }),
                           enemyBullets.end());
    }
}

void Game::render()
{
    renderer->clear();

    if (inMenu)
    {
        menu->render();
    }
    else
    {
        level->render(renderer->getSDLRenderer(), camera.x, camera.y);
        renderEnemies(renderer->getSDLRenderer(), camera.x, camera.y);

        for (auto &weapon : droppedWeapons)
        {
            if (!weapon)
                continue;
            // Get weapon's world position
            float weaponX = weapon->getX();
            float weaponY = weapon->getY();

            // Convert to screen coordinates using camera offset
            int screenX = static_cast<int>(weaponX - camera.x);
            int screenY = static_cast<int>(weaponY - camera.y);

            // Render as dropped
            weapon->render(renderer->getSDLRenderer(), static_cast<float>(screenX), static_cast<float>(screenY), 0.0f, true);
        }

        player->render(renderer->getSDLRenderer(), camera.x, camera.y);

        for (auto &bullet : enemyBullets)
            bullet.render(renderer->getSDLRenderer(), camera.x, camera.y);
    }

    renderer->present();
}

void Game::clean()
{
    // Clear enemy objects and bullets.
    enemies.clear();
    enemyBullets.clear();

    // Clear ResourceManager to free all textures.
    ResourceManager::clear();

    IMG_Quit();
    SDL_Quit();
}

bool Game::isRunning() const
{
    return running;
}

void Game::spawnEnemies(SDL_Renderer *renderer)
{
    // For testing: spawn two enemies at fixed positions.
    enemies.push_back(std::make_unique<Enemy>(400, 500, renderer));
    enemies.push_back(std::make_unique<Enemy>(200, 200, renderer));
}

void Game::renderEnemies(SDL_Renderer *renderer, int cameraX, int cameraY)
{
    for (auto &enemy : enemies)
    {
        enemy->render(renderer, cameraX, cameraY);
    }
}
