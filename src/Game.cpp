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
    if (!createWindowAndRenderer(title, width, height))
        return false;

    SDL_Renderer *sdlRenderer = renderer->getSDLRenderer();
    menu = std::make_unique<Menu>(sdlRenderer);
    level = std::make_unique<Level>(sdlRenderer, "assets/map/map.json");
    player = std::make_unique<Player>(sdlRenderer, level.get());

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
    else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        player->shoot(mouseX, mouseY, camera.x, camera.y);
    }
    // Additional game-specific event processing can be added here.
}

void Game::handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
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

        player->update(SCREEN_WIDTH, SCREEN_HEIGHT);
        updateEnemies(1.0f / 60.0f);
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
        player->render(renderer->getSDLRenderer(), camera.x, camera.y);
    }

    renderer->present();
}

void Game::clean()
{
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
    enemies.push_back(std::make_unique<Enemy>(500, 500, renderer));
    enemies.push_back(std::make_unique<Enemy>(300, 400, renderer));
}

void Game::updateEnemies(float dt)
{
    // Get the player's on-screen rectangle for enemy targeting.
    SDL_Rect playerRect;
    playerRect.x = static_cast<int>(player->getX());
    playerRect.y = static_cast<int>(player->getY());
    playerRect.w = player->getWidth();
    playerRect.h = player->getHeight();

    // Update each enemy.
    for (auto &enemy : enemies)
    {
        enemy->update(dt, playerRect, level->getCollisionTiles());
    }
    // Remove enemies that are dead if you want (or keep corpses rendered separately).
    // For now, we leave them in the vector to render the corpse.
}

void Game::renderEnemies(SDL_Renderer *renderer, int cameraX, int cameraY)
{
    for (auto &enemy : enemies)
    {
        enemy->render(renderer, cameraX, cameraY);
    }
}
