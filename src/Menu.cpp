#include "Menu.h"
#include <SDL2/SDL_image.h>
#include <cstdio>
#include <utility> // for std::move

#include "Constants.h"
static const int buttonWidth = BUTTON_WIDTH;
static const int buttonHeight = BUTTON_HEIGHT;
static const int centerX = MENU_CENTER_X; // (SCREEN_WIDTH - BUTTON_WIDTH) / 2
static const int startY = MENU_START_Y;
static const int spacing = MENU_SPACING;

Menu::Menu(SDL_Renderer *renderer) : renderer(renderer)
{
    // Load background texture.
    background = loadTexture("assets/menu/menu_background.png");

    // Clear the buttons vector (if not empty) and then add each button using emplace_back.
    buttons.clear();
    buttons.emplace_back(
        Button{
            std::move(loadTexture("assets/menu/start_button.png")),
            std::move(loadTexture("assets/menu/start_button_hover.png")),
            {centerX, startY, buttonWidth, buttonHeight},
            false});
    buttons.emplace_back(
        Button{
            std::move(loadTexture("assets/menu/manual_button.png")),
            std::move(loadTexture("assets/menu/manual_button_hover.png")),
            {centerX, startY + spacing, buttonWidth, buttonHeight},
            false});
    buttons.emplace_back(
        Button{
            std::move(loadTexture("assets/menu/exit_button.png")),
            std::move(loadTexture("assets/menu/exit_button_hover.png")),
            {centerX, startY + 2 * spacing, buttonWidth, buttonHeight},
            false});
}

Menu::~Menu()
{
    // Unique pointers automatically destroy textures.
    // No manual cleanup required.
}

std::unique_ptr<SDL_Texture, SDLTextureDeleter> Menu::loadTexture(const std::string &path)
{
    // Load texture and check for errors.
    SDL_Texture *tex = IMG_LoadTexture(renderer, path.c_str());
    if (!tex)
    {
        printf("Failed to load texture: %s, SDL_Error: %s\n", path.c_str(), SDL_GetError());
        return nullptr;
    }
    // Wrap the raw pointer in a unique_ptr with our custom deleter.
    return std::unique_ptr<SDL_Texture, SDLTextureDeleter>(tex);
}

void Menu::handleEvents(SDL_Event &event, bool &running, bool &inMenu)
{
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Update hover state for each button.
    for (auto &button : buttons)
    {
        button.hovered = (mouseX >= button.rect.x && mouseX <= button.rect.x + button.rect.w &&
                          mouseY >= button.rect.y && mouseY <= button.rect.y + button.rect.h);
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        // Check which button was clicked.
        for (size_t i = 0; i < buttons.size(); ++i)
        {
            if (buttons[i].hovered)
            {
                if (i == 0)
                    inMenu = false; // Start game.
                if (i == 1)
                {
                    // Implement manual display.
                }
                if (i == 2)
                    running = false; // Exit game.
            }
        }
    }
}

void Menu::render()
{
    // Render the background covering the whole window.
    SDL_RenderCopy(renderer, background.get(), nullptr, nullptr);

    // Render each button, using the hover texture if hovered.
    for (const auto &button : buttons)
    {
        SDL_RenderCopy(renderer, button.hovered ? button.hoverTexture.get() : button.texture.get(), nullptr, &button.rect);
    }
}
