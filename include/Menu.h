#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>
#include <memory>
#include "SDLDeleters.h"

struct Button
{
    // Use unique_ptr to manage textures
    std::unique_ptr<SDL_Texture, SDLTextureDeleter> texture;
    std::unique_ptr<SDL_Texture, SDLTextureDeleter> hoverTexture;
    SDL_Rect rect;
    bool hovered = false;
};

class Menu
{
public:
    Menu(SDL_Renderer *renderer);
    ~Menu();
    void handleEvents(SDL_Event &event, bool &running, bool &inMenu);
    void render();

private:
    SDL_Renderer *renderer; // Not owned by Menu.
    std::unique_ptr<SDL_Texture, SDLTextureDeleter> background;
    std::vector<Button> buttons;

    std::unique_ptr<SDL_Texture, SDLTextureDeleter> loadTexture(const std::string &path);
};

#endif // MENU_H
