// Menu.cpp
#include "Menu.h"
#include "Constants.h"

Menu::Menu(SDL_Renderer *renderer) : renderer(renderer)
{
    background = ResourceManager::loadTexture(renderer, "assets/menu/menu_background.png");
    initButtons(renderer);
}

Menu::~Menu()
{
    // ResourceManager manages textures; call clear() at program shutdown if desired.
}

void Menu::initButtons(SDL_Renderer *renderer)
{
    int centerX = MENU_CENTER_X; // Defined in Constants.h
    int startY = MENU_START_Y;
    int buttonWidth = BUTTON_WIDTH;
    int buttonHeight = BUTTON_HEIGHT;
    int spacing = MENU_SPACING;

    buttons.emplace_back("assets/menu/start_button.png", "assets/menu/start_button_hover.png",
                         SDL_Rect{centerX, startY, buttonWidth, buttonHeight}, renderer);
    buttons.emplace_back("assets/menu/manual_button.png", "assets/menu/manual_button_hover.png",
                         SDL_Rect{centerX, startY + spacing, buttonWidth, buttonHeight}, renderer);
    buttons.emplace_back("assets/menu/exit_button.png", "assets/menu/exit_button_hover.png",
                         SDL_Rect{centerX, startY + 2 * spacing, buttonWidth, buttonHeight}, renderer);
}

void Menu::handleEvents(SDL_Event &event, bool &running, bool &inMenu)
{
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Update each button's hover state.
    for (auto &button : buttons)
    {
        button.updateHover(mouseX, mouseY);
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        for (size_t i = 0; i < buttons.size(); ++i)
        {
            if (buttons[i].getHovered())
            {
                if (i == 0)
                    inMenu = false; // Start game.
                else if (i == 1)
                {
                    // Future: display manual.
                }
                else if (i == 2)
                    running = false; // Exit game.
            }
        }
    }
}

void Menu::render()
{
    // Render background.
    SDL_RenderCopy(renderer, background, nullptr, nullptr);
    // Render buttons.
    for (const auto &button : buttons)
    {
        button.render(renderer);
    }
}
