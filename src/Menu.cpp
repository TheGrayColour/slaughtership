// Menu.cpp
#include "Menu.h"
#include "Constants.h"

Menu::Menu(SDL_Renderer *renderer, bool pause) : renderer(renderer), isPause(pause)
{
    if (!isPause)
    {
        background = ResourceManager::loadTexture(renderer, "assets/menu/menu_background.png");
    }
    else
    {
        background = nullptr;
    }

    initButtons(renderer);
}

Menu::~Menu()
{
    // ResourceManager manages textures; call clear() at program shutdown if desired.
}

void Menu::initButtons(SDL_Renderer *renderer)
{
    buttons.clear();
    if (isPause)
    {
        // For pause menu, use resume and back buttons.
        int centerX = MENU_CENTER_X;
        int startY = MENU_START_Y;
        // Create a "Resume" button.
        buttons.emplace_back("assets/menu/resume_button.png", "assets/menu/resume_button_hover.png",
                             SDL_Rect{centerX, startY, BUTTON_WIDTH, BUTTON_HEIGHT}, renderer);
        // Create a "Back" button.
        buttons.emplace_back("assets/menu/back_button.png", "assets/menu/back_button_hover.png",
                             SDL_Rect{centerX, startY + MENU_SPACING, BUTTON_WIDTH, BUTTON_HEIGHT}, renderer);
    }
    else
    {
        // Existing start menu button layout.
        int centerX = MENU_CENTER_X;
        int startY = MENU_START_Y;
        int spacing = MENU_SPACING;
        buttons.emplace_back("assets/menu/start_button.png", "assets/menu/start_button_hover.png",
                             SDL_Rect{centerX, startY, BUTTON_WIDTH, BUTTON_HEIGHT}, renderer);
        buttons.emplace_back("assets/menu/manual_button.png", "assets/menu/manual_button_hover.png",
                             SDL_Rect{centerX, startY + spacing, BUTTON_WIDTH, BUTTON_HEIGHT}, renderer);
        buttons.emplace_back("assets/menu/exit_button.png", "assets/menu/exit_button_hover.png",
                             SDL_Rect{centerX, startY + 2 * spacing, BUTTON_WIDTH, BUTTON_HEIGHT}, renderer);
    }
}

void Menu::handleEvents(SDL_Event &event, bool &running, bool &menuActive, bool &backToMain)
{
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    for (auto &button : buttons)
    {
        button.updateHover(mouseX, mouseY);
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        // For pause menu, assume:
        // - Button 0: Resume (set menuActive = false to unpause)
        // - Button 1: Back (set running = false and menuActive = true to go back to start menu)
        if (isPause)
        {
            if (buttons[0].getHovered())
            {
                // Resume game.
                menuActive = false;
            }
            else if (buttons[1].getHovered())
            {
                // Go back to start menu.
                backToMain = true;
                menuActive = false;
            }
        }
        else
        {
            // Existing main menu logic...
            for (size_t i = 0; i < buttons.size(); ++i)
            {
                if (buttons[i].getHovered())
                {
                    if (i == 0)
                        menuActive = false; // Start game.
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
}

void Menu::render()
{
    // Render background.
    if (!isPause && background)
    {
        SDL_RenderCopy(renderer, background, nullptr, nullptr);
    }
    // Render buttons.
    for (const auto &button : buttons)
    {
        button.render(renderer);
    }
}
