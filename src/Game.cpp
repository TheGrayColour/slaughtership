#include "Game.h"
#include "SDL2/SDL_image.h"
#include <iostream>
#include "Constants.h"

Game::Game() : running(false), inMenu(true),
               camera{0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}
{
}

Game::~Game()
{
    clean(); // clean() now mainly handles SDL_Quit/IMG_Quit
}

bool Game::init(const char *title, int width, int height)
{
    SDL_Window *rawWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!rawWindow)
    {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return false;
    }
    window.reset(rawWindow);

    // Create the renderer object instead of managing SDL_Renderer directly
    renderer = std::make_unique<Renderer>(window.get());

    // Pass the renderer to game objects
    menu = std::make_unique<Menu>(renderer->getSDLRenderer());
    level = std::make_unique<Level>(renderer->getSDLRenderer(), "assets/map/map.json");
    player = std::make_unique<Player>(renderer->getSDLRenderer(), level.get());

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
            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                player->shoot(mouseX, mouseY, camera.x, camera.y);
            }

            // Get the current key states and update player's movement using InputManager.
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
        // Keep the camera centered on the player
        camera.x = player->getX() - camera.w / 2;
        camera.y = player->getY() - camera.h / 2;

        player->update(SCREEN_WIDTH, SCREEN_HEIGHT);
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
        player->render(renderer->getSDLRenderer(), camera.x, camera.y);
    }

    renderer->present();
}

void Game::clean()
{
    // Smart pointers automatically clean up window, renderer, player, level, and menu.
    IMG_Quit();
    SDL_Quit();
}

bool Game::isRunning() const
{
    return running;
}
