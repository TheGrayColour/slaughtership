#include "Renderer.h"
#include <iostream>

Renderer::Renderer(SDL_Window *window)
{
    SDL_Renderer *rawRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!rawRenderer)
    {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        throw std::runtime_error("Renderer creation failed");
    }
    renderer.reset(rawRenderer);
}

Renderer::~Renderer()
{
    // Smart pointer automatically cleans up the renderer
}

void Renderer::clear()
{
    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(renderer.get());
}

void Renderer::present()
{
    SDL_RenderPresent(renderer.get());
}

void Renderer::drawTexture(SDL_Texture *texture, const SDL_Rect *src, const SDL_Rect *dst, double angle, SDL_Point *center, SDL_RendererFlip flip)
{
    SDL_RenderCopyEx(renderer.get(), texture, src, dst, angle, center, flip);
}
