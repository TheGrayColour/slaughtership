#include "Game.h"
#include "SDL2/SDL_image.h"
#include <iostream>
#include "Constants.h"
#include <cmath>
#include "ResourceManager.h"

Game::Game() : running(false), currentState(GameState::MAIN_MENU), camera{0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, showingManual(false) {}

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
    mapFiles = {"assets/map/map0.json", "assets/map/map1.json", "assets/map/map2.json", "assets/map/map3.json"};
    cutsceneFiles = {"assets/cutscenes/cutscene0.mp4", "assets/cutscenes/cutscene1.mp4", "assets/cutscenes/cutscene2.mp4", "assets/cutscenes/cutscene3.mp4"};
    currentMapIndex = 0;

    // Clear any previous data.
    enemies.clear();
    enemyBullets.clear();
    droppedWeapons.clear();
    ResourceManager::clear();

    if (!createWindowAndRenderer(title, width, height))
        return false;

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to init SDL_image PNG support: %s",
                     IMG_GetError());
        return false;
    }

    SDL_Renderer *sdlRenderer = renderer->getSDLRenderer();
    menu = std::make_unique<Menu>(sdlRenderer);
    skipTexture = ResourceManager::loadTexture(sdlRenderer, "assets/menu/skip_button.png");

    manualTexture = ResourceManager::loadTexture(sdlRenderer, "assets/menu/manual.png");
    if (!manualTexture)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to load manual.png via ResourceManager");
    }

    if (!manualTexture)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to load menu/manual.png: %s",
                     IMG_GetError());
    }

    // don't load map0 until the player actually starts
    currentState = GameState::MAIN_MENU;
    pauseMenu.reset();

    // Initialize cutscene manager.
    cutsceneManager = std::make_unique<CutsceneManager>();

    running = true;
    return true;
}

void Game::startCutscene(const std::string &filePath)
{
    // ⟵ patched: remove existing enemies/bullets/weapons before switching to CUTSCENE
    enemies.clear();
    enemyBullets.clear();
    droppedWeapons.clear();

    // 1) Create a fresh manager (drops any old state)
    cutsceneManager = std::make_unique<CutsceneManager>();

    // ❏ Reload skip‐button via SDL_image to avoid stale cache
    if (skipTexture)
    {
        SDL_DestroyTexture(skipTexture);
        skipTexture = nullptr;
    }
    skipTexture = IMG_LoadTexture(
        renderer->getSDLRenderer(),
        "assets/menu/skip_button.png");

    // 3) Try to load the video
    if (!cutsceneManager->loadVideo(filePath, renderer->getSDLRenderer()))
    {

        // Immediately advance to the next map
        if (currentMapIndex + 1 < static_cast<int>(mapFiles.size()))
        {
            currentMapIndex++;
            restartLevel(renderer->getSDLRenderer());
        }
        else
        {
            // No more maps → exit
            running = false;
        }
        currentState = GameState::PLAYING;
        return;
    }

    // 4) All good → switch state and start playback
    currentState = GameState::CUTSCENE;
    cutsceneManager->play();
}

void Game::restartLevel(SDL_Renderer *sdlRenderer)
{
    // Clear level-specific objects.
    enemies.clear();
    enemyBullets.clear();
    droppedWeapons.clear();

    ResourceManager::clear();

    // Reinitialize the level using the current map.
    level = std::make_unique<Level>(sdlRenderer, mapFiles[currentMapIndex]);

    // Reinitialize the player.
    player = std::make_unique<Player>(sdlRenderer, level.get());
    SDL_FPoint ps = level->getPlayerSpawn();
    player->setPosition(ps.x, ps.y);

    // Respawn enemies for the new level.
    if (currentMapIndex > 0)
        spawnEnemies(sdlRenderer);
}

void Game::processGameInput(SDL_Event &event)
{

    if (event.type == SDL_QUIT)
    {
        running = false;
        return;
    }

    if (currentState == GameState::CUTSCENE)
    {
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_x)
        {
            cutsceneManager->skip();
        }
        return;
    }

    if (currentState == GameState::PLAYING)
    {
        // Process left-click for shooting.
        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
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
}

void Game::handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // 1) Always allow quitting via window close
        if (event.type == SDL_QUIT)
        {
            running = false;
            break;
        }

        // 2) If we're showing the manual overlay, any click or key closes it
        if (currentState == GameState::MAIN_MENU && showingManual)
        {
            if (event.type == SDL_MOUSEBUTTONDOWN ||
                event.type == SDL_KEYDOWN)
            {
                showingManual = false;
            }
            // Don’t process any menu buttons while the manual is up
            continue;
        }

        // 3) ESC toggles between PLAYING <-> PAUSED
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
        {
            if (currentState == GameState::PLAYING)
            {
                currentState = GameState::PAUSED;
                pauseMenu = std::make_unique<Menu>(renderer->getSDLRenderer(), true);
            }
            else if (currentState == GameState::PAUSED)
            {
                currentState = GameState::PLAYING;
                pauseMenu.reset();
            }
            continue;
        }

        // 4) Shared menu handling for MAIN_MENU & PAUSED
        if (currentState == GameState::MAIN_MENU || currentState == GameState::PAUSED)
        {
            Menu *activeMenu = (currentState == GameState::MAIN_MENU)
                                   ? menu.get()
                                   : pauseMenu.get();

            MenuAction action = activeMenu->handleEvents(event, running);

            switch (action)
            {
            case MenuAction::NONE:
                break;

            case MenuAction::EXIT:
                running = false;
                break;

            case MenuAction::SPECIAL:
                if (currentState == GameState::MAIN_MENU)
                {
                    // Manual button clicked
                    showingManual = true;
                }
                else
                {
                    // Back to main from pause
                    currentState = GameState::MAIN_MENU;
                    inMenu = true;
                    showingManual = false;
                    pauseMenu.reset();

                    // Reset all gameplay state and resources
                    enemies.clear();
                    enemyBullets.clear();
                    droppedWeapons.clear();
                    ResourceManager::clear();
                    level.reset();
                    player.reset();

                    // Recreate the main menu UI (so its buttons are in fresh state)
                    menu = std::make_unique<Menu>(renderer->getSDLRenderer());
                }
                return;

            case MenuAction::ACCEPT:
                if (currentState == GameState::MAIN_MENU)
                {
                    // Start clicked
                    currentMapIndex = 0;
                    restartLevel(renderer->getSDLRenderer());
                    currentState = GameState::PLAYING;
                }
                else
                {
                    // Resume clicked
                    currentState = GameState::PLAYING;
                    pauseMenu.reset();
                }
                break;
            }

            // Don’t fall through to gameplay input
            continue;
        }

        // 5) CUTSCENE: forward to processGameInput for skipping, etc.
        if (currentState == GameState::CUTSCENE)
        {
            processGameInput(event);
            continue;
        }

        if (currentState == GameState::PLAYING)
        {
            // 6) Special in-game key: 'T' triggers first cutscene
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_t && currentMapIndex == 0)
            {
                SDL_Rect phone = level->getPhoneTrigger();
                SDL_Rect playerRect = {
                    static_cast<int>(player->getX()),
                    static_cast<int>(player->getY()),
                    static_cast<int>(player->getWidth()),
                    static_cast<int>(player->getHeight())};
                if (SDL_HasIntersection(&playerRect, &phone))
                {
                    startCutscene(cutsceneFiles[0]);
                    continue;
                }
            }

            // 7) Restart on death with 'R'
            if (player->isDead() && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r)
            {
                restartLevel(renderer->getSDLRenderer());
                continue;
            }
        }

        // 8) All other in-game input
        processGameInput(event);

        // 9) Continuous key state for player movement
        const Uint8 *keys = SDL_GetKeyboardState(nullptr);
        player->updateInput(keys);

        // 10) Update player facing angle
        if (!player->isDead())
        {
            int mx, my;
            SDL_GetMouseState(&mx, &my);
            float cx = player->getX() + player->getWidth() / 2;
            float cy = player->getY() + player->getHeight() / 2;
            float dx = (mx + camera.x) - cx;
            float dy = (my + camera.y) - cy;
            player->setAngle(atan2(dy, dx) * (180.0f / M_PI));
        }
    }
}

void Game::update()
{

    // 1) If in cutscene, advance when done
    if (currentState == GameState::CUTSCENE)
    {
        bool finished = cutsceneManager->update();
        if (finished)
        {

            // advance only if there's another map
            if (currentMapIndex + 1 < static_cast<int>(mapFiles.size()))
            {
                currentMapIndex++;
                restartLevel(renderer->getSDLRenderer());
            }
            else
            {
                // no more maps => exit
                running = false;
            }
            currentState = GameState::PLAYING;
        }
        return;
    }

    if (currentState != GameState::PLAYING)
        return;

    // Normal gameplay update.
    int desiredX = player->getX() - camera.w / 2;
    int desiredY = player->getY() - camera.h / 2;
    float smoothingFactor = 0.1f;
    camera.x = static_cast<int>(camera.x + smoothingFactor * (desiredX - camera.x));
    camera.y = static_cast<int>(camera.y + smoothingFactor * (desiredY - camera.y));

    player->update(SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_Rect playerRect = {static_cast<int>(player->getX()),
                           static_cast<int>(player->getY()),
                           static_cast<int>(player->getWidth()),
                           static_cast<int>(player->getHeight())};

    bool meleeAttack = false;
    if (!player->getWeapons()->hasWeapon())
    {
        meleeAttack = player->getAnimation()->isAttacking();
    }
    else if (player->getWeapons()->isMeleeWeapon() && player->getWeapons()->isAttacking())
    {
        meleeAttack = true;
    }
    if (!player->isDead() && meleeAttack)
    {
        SDL_Rect meleeArea = {static_cast<int>(player->getX()) + PLAYER_COLLISION_OFFSET_X,
                              static_cast<int>(player->getY()) + PLAYER_COLLISION_OFFSET_Y,
                              PLAYER_COLLISION_WIDTH, PLAYER_COLLISION_HEIGHT};
        for (auto &enemy : enemies)
        {
            SDL_Rect enemyBox = enemy->getCollisionBox();
            if (!enemy->isDead() && SDL_HasIntersection(&meleeArea, &enemyBox))
            {
                enemy->takeDamage(9999);
            }
        }
    }

    for (auto &enemy : enemies)
    {
        enemy->update(1.0f / 60.0f, playerRect, level->getCollisionTiles(), enemyBullets, !player->isDead());
    }

    for (auto &enemy : enemies)
    {
        if (enemy->isDead() && enemy->hasWeapon())
        {
            droppedWeapons.push_back(enemy->dropWeapon());
        }
    }

    for (auto &bullet : enemyBullets)
    {
        bullet.update(1.0f, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    SDL_Rect playerCollision = {static_cast<int>(player->getX()) + PLAYER_COLLISION_OFFSET_X,
                                static_cast<int>(player->getY()) + PLAYER_COLLISION_OFFSET_Y,
                                PLAYER_COLLISION_WIDTH, PLAYER_COLLISION_HEIGHT};
    for (auto &bullet : enemyBullets)
    {
        SDL_Rect bulletRect = {static_cast<int>(bullet.getX()), static_cast<int>(bullet.getY()), 5, 5};
        for (const auto &wall : level->getCollisionTiles())
        {
            if (SDL_HasIntersection(&bulletRect, &wall))
            {
                bullet.deactivate();
                break;
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
        for (const auto &wall : level->getCollisionTiles())
        {
            if (SDL_HasIntersection(&bulletRect, &wall))
            {
                bullet.deactivate();
                break;
            }
        }
        for (auto &enemy : enemies)
        {
            SDL_Rect enemyBox = enemy->getCollisionBox();
            if (!enemy->isDead() && SDL_HasIntersection(&bulletRect, &enemyBox))
            {
                enemy->takeDamage(9999);
                bullet.deactivate();
                break;
            }
        }
    }

    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                 [](const Bullet &b)
                                 { return !b.isActive(); }),
                  bullets.end());
    enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(),
                                      [](const Bullet &b)
                                      { return !b.isActive(); }),
                       enemyBullets.end());

    // 3) Check for “all enemies dead” only on maps ≥1, and only if there *is* a cutscene for this map.
    if (currentState == GameState::PLAYING && currentMapIndex > 0 && currentMapIndex < static_cast<int>(cutsceneFiles.size()))
    {
        bool anyAlive = false;
        for (auto &e : enemies)
            if (!e->isDead())
            {
                anyAlive = true;
                break;
            }

        if (!anyAlive)
        {
            // trigger the cutscene for this map, then defer advancing until it finishes
            startCutscene(cutsceneFiles[currentMapIndex]);
            return;
        }
    }
}

void Game::render()
{
    renderer->clear();

    if (currentState == GameState::MAIN_MENU)
    {
        if (showingManual)
        {
            SDL_Rect full{0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(
                renderer->getSDLRenderer(),
                manualTexture,
                nullptr,
                &full);
        }
        else
        {
            menu->render();
        }

        renderer->present();
        return;
    }

    level->render(renderer->getSDLRenderer(), camera.x, camera.y);

    // debug
    // if (currentMapIndex == 0)
    // {
    //     SDL_Renderer *rd = renderer->getSDLRenderer();
    //     SDL_SetRenderDrawBlendMode(rd, SDL_BLENDMODE_BLEND);
    //     // red, 50% alpha
    //     SDL_SetRenderDrawColor(rd, 255, 0, 0, 128);

    //     SDL_Rect phone = level->getPhoneTrigger();
    //     SDL_Rect dbg = {
    //         phone.x - camera.x,
    //         phone.y - camera.y,
    //         phone.w,
    //         phone.h};
    //     SDL_RenderFillRect(rd, &dbg);

    //     // (optional) outline instead of fill:
    //     SDL_SetRenderDrawColor(rd, 255, 0, 0, 255);
    //     SDL_RenderDrawRect(rd, &dbg);
    // }

    renderEnemies(renderer->getSDLRenderer(), camera.x, camera.y);
    for (auto &weapon : droppedWeapons)
    {
        if (!weapon)
            continue;
        float weaponX = weapon->getX();
        float weaponY = weapon->getY();
        int screenX = static_cast<int>(weaponX - camera.x);
        int screenY = static_cast<int>(weaponY - camera.y);
        // Render dropped weapon (rendering as dropped, so pass true for dropped flag).
        weapon->render(renderer->getSDLRenderer(), static_cast<float>(screenX), static_cast<float>(screenY), 0.0f, true);
    }
    player->render(renderer->getSDLRenderer(), camera.x, camera.y);
    for (auto &bullet : enemyBullets)
        bullet.render(renderer->getSDLRenderer(), camera.x, camera.y);

    // If state is CUTSCENE, render the cutscene overlay.
    if (currentState == GameState::CUTSCENE)
    {
        cutsceneManager->render(renderer->getSDLRenderer());

        // draw skip-button in upper right
        if (skipTexture)
        {
            SDL_Renderer *rd = renderer->getSDLRenderer();
            int tw, th;
            SDL_QueryTexture(skipTexture, nullptr, nullptr, &tw, &th);
            // 16px margin from top/right
            SDL_Rect dst = {SCREEN_WIDTH - tw - 32, 32, tw, th};
            // ensure blending so semi-transparent text shows over video
            SDL_SetRenderDrawBlendMode(rd, SDL_BLENDMODE_BLEND);
            SDL_RenderCopy(rd, skipTexture, nullptr, &dst);
        }
    }

    // If paused, render the pause overlay and menu.
    if (currentState == GameState::PAUSED && pauseMenu)
    {
        SDL_SetRenderDrawBlendMode(renderer->getSDLRenderer(), SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer->getSDLRenderer(), 0, 0, 0, 150);
        SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer->getSDLRenderer(), &overlay);
        pauseMenu->render();
    }

    renderer->present();
}

void Game::clean()
{
    // Clear enemy objects and bullets.
    enemies.clear();
    enemyBullets.clear();
    droppedWeapons.clear();

    // Clear ResourceManager to free all textures.
    ResourceManager::clear();

    if (skipTexture)
    {
        SDL_DestroyTexture(skipTexture);
        skipTexture = nullptr;
    }

    if (manualTexture)
    {
        SDL_DestroyTexture(manualTexture);
        manualTexture = nullptr;
    }

    IMG_Quit();
    SDL_Quit();
}

bool Game::isRunning() const
{
    return running;
}

void Game::spawnEnemies(SDL_Renderer *renderer)
{
    for (auto &es : level->getEnemySpawns())
    {
        for (int i = 0; i < es.count; ++i)
        {
            if (es.type == "Normal")
                enemies.push_back(std::make_unique<Enemy>(es.x, es.y, renderer));
            else if (es.type == "Boss")
                enemies.push_back(std::make_unique<BossEnemy>(es.x, es.y, renderer));
            else if (es.type == "FinalBoss")
                enemies.push_back(std::make_unique<FinalBossEnemy>(es.x, es.y, renderer));
            else
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                            "Unknown enemy type '%s' at (%f,%f)",
                            es.type.c_str(), es.x, es.y);
        }
    }
}

void Game::renderEnemies(SDL_Renderer *renderer, int cameraX, int cameraY)
{
    for (auto &enemy : enemies)
    {
        enemy->render(renderer, cameraX, cameraY);
    }
}
