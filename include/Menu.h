// Menu.h
#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "SDLDeleters.h"
#include "ResourceManager.h"

// Button abstraction with its own hover update.
class Button
{
public:
    Button(const std::string &normalPath, const std::string &hoverPath, const SDL_Rect &rect, SDL_Renderer *renderer)
        : rect(rect), hovered(false)
    {
        texture = ResourceManager::loadTexture(renderer, normalPath);
        hoverTexture = ResourceManager::loadTexture(renderer, hoverPath);
    }

    // Update the hover state based on current mouse position.
    void updateHover(int mouseX, int mouseY)
    {
        hovered = (mouseX >= rect.x && mouseX <= rect.x + rect.w &&
                   mouseY >= rect.y && mouseY <= rect.y + rect.h);
    }

    // Render the button using the appropriate texture.
    void render(SDL_Renderer *renderer) const
    {
        SDL_Texture *tex = hovered && hoverTexture ? hoverTexture : texture;
        SDL_RenderCopy(renderer, tex, nullptr, &rect);
    }

    bool getHovered() const { return hovered; }

private:
    SDL_Rect rect;             // Now declared first.
    bool hovered;              // Declared after rect.
    SDL_Texture *texture;      // Not owned; managed by ResourceManager.
    SDL_Texture *hoverTexture; // Not owned; managed by ResourceManager.
};

class Menu
{
public:
    Menu(SDL_Renderer *renderer, bool pause = false);
    ~Menu();

    void handleEvents(SDL_Event &event, bool &running, bool &menuActive, bool &backToMain);
    void render();

private:
    SDL_Renderer *renderer;  // Not owned.
    SDL_Texture *background; // Loaded via ResourceManager.
    std::vector<Button> buttons;
    bool isPause;

    // Initializes button layout.
    void initButtons(SDL_Renderer *renderer);
};

#endif // MENU_H
