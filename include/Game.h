#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Level.h"
#include "Menu.h"

struct Camera
{
    int x, y, w, h;
};

class Game
{
public:
    Game();
    ~Game();

    bool init(const char *title, int width, int height);
    void handleEvents();
    void update();
    void render();
    void clean();
    bool isRunning() const;

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool running;
    bool inMenu;

    Player *player; // Use a pointer for late initialization
    Level *level;
    Menu *menu;

    Camera camera;
};

#endif // GAME_H
