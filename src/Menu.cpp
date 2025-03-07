#include "Menu.h"
#include <SDL2/SDL_image.h>

Menu::Menu(SDL_Renderer *renderer) : renderer(renderer)
{
    // Load background
    background = loadTexture("assets/menu/menu_background.png");

    // Load buttons
    buttons = {
        {loadTexture("assets/menu/start_button.png"), loadTexture("assets/menu/start_button_hover.png"), {300, 200, 200, 50}},
        {loadTexture("assets/menu/manual_button.png"), loadTexture("assets/menu/manual_button_hover.png"), {300, 300, 200, 50}},
        {loadTexture("assets/menu/exit_button.png"), loadTexture("assets/menu/exit_button_hover.png"), {300, 400, 200, 50}}};
}

Menu::~Menu()
{
    SDL_DestroyTexture(background);
    for (auto &button : buttons)
    {
        SDL_DestroyTexture(button.texture);
        SDL_DestroyTexture(button.hoverTexture);
    }
}

SDL_Texture *Menu::loadTexture(const std::string &path)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer, path.c_str());
    if (!texture)
    {
        printf("Failed to load texture: %s\n", SDL_GetError());
    }
    return texture;
}

void Menu::handleEvents(SDL_Event &event, bool &running, bool &inMenu)
{
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    for (auto &button : buttons)
    {
        button.hovered = (mouseX >= button.rect.x && mouseX <= button.rect.x + button.rect.w &&
                          mouseY >= button.rect.y && mouseY <= button.rect.y + button.rect.h);
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        for (size_t i = 0; i < buttons.size(); ++i)
        {
            if (buttons[i].hovered)
            {
                if (i == 0)
                    inMenu = false; // Start game
                if (i == 1)
                {
                } // Show manual (implement separately)
                if (i == 2)
                    running = false; // Exit
            }
        }
    }
}

void Menu::render()
{
    SDL_RenderCopy(renderer, background, nullptr, nullptr); // Draw background

    for (const auto &button : buttons)
    {
        SDL_RenderCopy(renderer, button.hovered ? button.hoverTexture : button.texture, nullptr, &button.rect);
    }
}
