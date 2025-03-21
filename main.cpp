#define SDL_MAIN_HANDLED
#include <Game.h>
#include <SDL2/SDL.h>

int main(int argc, char *argv[])
{
    Game game;

    if (!game.init("slaughtership", 1200, 768))
    {
        return -1;
    }

    const int FPS = 60;
    const int frameDelay = 1000 / FPS; // 16.67ms per framef

    Uint32 frameStart;
    int frameTime;

    while (game.isRunning())
    {
        frameStart = SDL_GetTicks(); // Get the start time of the frame

        game.handleEvents();
        game.update();
        game.render();

        // Calculate how long the frame took
        frameTime = SDL_GetTicks() - frameStart;

        // Delay to maintain 60 FPS
        if (frameTime < frameDelay)
        {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    game.clean();
    return 0;
}
