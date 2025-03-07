#define SDL_MAIN_HANDLED
#include "Game.h"

int main(int argc, char *argv[])
{
    Game game;

    if (!game.init("slaughtership", 800, 600))
    {
        return -1;
    }

    while (game.isRunning())
    {
        game.handleEvents();
        game.update();
        game.render();

        SDL_Delay(8); // ~60 FPS
    }

    game.clean();
    return 0;
}
