// Game.h
#ifndef GAME_H
#define GAME_H

#include <memory>
#include "Renderer.h"
#include "Player.h"
#include "Level.h"
#include "Menu.h"
#include "InputManager.h"
#include "SDLDeleters.h"
#include "Enemy.h"
#include "BossEnemy.h"
#include "CutsceneManager.h"

enum class GameState
{
    MAIN_MENU,
    PLAYING,
    PAUSED,
    CUTSCENE
};

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

    void startCutscene(const std::string &filePath);

private:
    bool createWindowAndRenderer(const char *title, int width, int height);
    void processGameInput(SDL_Event &event); // New: dedicated game input handler

    std::unique_ptr<SDL_Window, SDLWindowDeleter> window;
    std::unique_ptr<Renderer> renderer;
    bool running;
    bool inMenu;

    std::unique_ptr<Player> player;
    std::unique_ptr<Level> level;
    std::unique_ptr<Menu> menu;

    GameState currentState;
    std::unique_ptr<Menu> pauseMenu;
    std::unique_ptr<CutsceneManager> cutsceneManager;

    SDL_Texture *skipTexture = nullptr;

    // Manual‐screen overlay in the menu
    SDL_Texture *manualTexture = nullptr;
    bool showingManual = false;

    Camera camera;

    std::vector<std::unique_ptr<Enemy>> enemies;

    void spawnEnemies(SDL_Renderer *renderer);
    void updateEnemies(float dt);
    void renderEnemies(SDL_Renderer *renderer, int cameraX, int cameraY);

    std::vector<Bullet> enemyBullets;

    std::vector<std::unique_ptr<AbstractWeapon>> droppedWeapons;

    // New members for level progression.
    std::vector<std::string> mapFiles;
    std::vector<std::string> cutsceneFiles;
    int currentMapIndex;

    // New method to restart the current level.
    void restartLevel(SDL_Renderer *sdlRenderer);
};

#endif // GAME_H
