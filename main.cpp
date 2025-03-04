#include "Game.h"

int main(int argc, char *argv[])
{
    Game game;

    if (!game.init("Hotline Miami Clone", 800, 600))
    {
        return -1;
    }

    while (game.isRunning())
    {
        game.handleEvents();
        game.update();
        game.render();

        SDL_Delay(16); // ~60 FPS
    }

    game.clean();
    return 0;
}
