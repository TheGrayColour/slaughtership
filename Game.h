#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "Player.h" // Include the Player class

class Game
{
public:
    Game() : window(nullptr), renderer(nullptr), running(true) {}

    bool init(const char *title, int width, int height)
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

        player = new Player(renderer); // Create the player object
        return true;
    }

    void handleEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }
    }

    void update()
    {
        const Uint8 *keys = SDL_GetKeyboardState(nullptr);
        player->handleInput(keys);
        player->update();
    }

    void render()
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear screen to black
        SDL_RenderClear(renderer);

        player->render(); // Draw the player

        SDL_RenderPresent(renderer);
    }

    void clean()
    {
        delete player;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
    }

    bool isRunning() const { return running; }

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    Player *player;
    bool running;
};
