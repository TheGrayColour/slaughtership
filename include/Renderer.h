#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <memory>
#include "SDLDeleters.h"

class Renderer
{
public:
    Renderer(SDL_Window *window);
    ~Renderer();

    SDL_Renderer *getSDLRenderer() { return renderer.get(); }
    void clear();
    void present();
    void drawTexture(SDL_Texture *texture, const SDL_Rect *src, const SDL_Rect *dst, double angle = 0, SDL_Point *center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);

private:
    std::unique_ptr<SDL_Renderer, SDLRendererDeleter> renderer;
};

#endif // RENDERER_H
