#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>

struct Button
{
    SDL_Texture *texture;
    SDL_Texture *hoverTexture;
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
    SDL_Renderer *renderer;
    SDL_Texture *background;
    std::vector<Button> buttons;

    SDL_Texture *loadTexture(const std::string &path);
};

#endif
