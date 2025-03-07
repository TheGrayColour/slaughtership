#include "Game.h"
#include <SDL2/SDL_image.h>
#include <iostream>
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

Game::Game() : window(nullptr), renderer(nullptr), running(false), inMenu(true), player(nullptr), level(nullptr), menu(nullptr),
               camera{0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}
{
}

Game::~Game()
{
    clean();
}

bool Game::init(const char *title, int width, int height)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!window)
    {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
        return false;
    }

    // Initialize menu after renderer is created
    menu = new Menu(renderer);

    // Initialize the player
    player = new Player(renderer); // Pass starting position
    level = new Level(renderer, "assets/map/map.json");

    running = true;
    return true;
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

            if (event.type == SDL_QUIT)
            {
                running = false;
            }

            // Get mouse position
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                player->shoot(mouseX, mouseY, camera.x, camera.y);
            }

            float playerCenterX = player->getX() + (player->getWidth() / 2);
            float playerCenterY = player->getY() + (player->getHeight() / 2);

            float dx = (mouseX + camera.x) - playerCenterX;
            float dy = (mouseY + camera.y) - playerCenterY;

            // Convert to degrees and fix alignment
            float targetAngle = atan2(dy, dx) * (180.0 / M_PI) + 90.0f;
            player->setAngle(targetAngle);
        }
    }

    const Uint8 *keys = SDL_GetKeyboardState(nullptr);
    player->handleInput(keys);
}

void Game::update()
{
    if (!inMenu)
    {
        // Keep the camera centered on the player
        camera.x = player->getX() - camera.w / 2;
        camera.y = player->getY() - camera.h / 2;

        player->update();
    }
}

void Game::render()
{

    SDL_RenderClear(renderer);
    if (inMenu)
    {
        menu->render();
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear screen to black
        level->render(renderer, camera.x, camera.y);
        player->render(renderer, camera.x, camera.y); // Draw the player
    }

    SDL_RenderPresent(renderer);
}

void Game::clean()
{
    delete menu;
    delete player;
    delete level;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

bool Game::isRunning() const
{
    return running;
}
