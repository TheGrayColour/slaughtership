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
        bool createWindowAndRenderer(const char *title, int width, int height);
        void processGameInput(SDL_Event &event); // New: dedicated game input handler

        std::unique_ptr<SDL_Window, SDLWindowDeleter> window;
        std::unique_ptr<Renderer> renderer;
        bool running;
        bool inMenu;

        std::unique_ptr<Player> player;
        std::unique_ptr<Level> level;
        std::unique_ptr<Menu> menu;

        Camera camera;

        std::vector<std::unique_ptr<Enemy>> enemies;

        void spawnEnemies(SDL_Renderer *renderer);
        void updateEnemies(float dt);
        void renderEnemies(SDL_Renderer *renderer, int cameraX, int cameraY);

        std::vector<Bullet> enemyBullets;
    };

    #endif // GAME_H
