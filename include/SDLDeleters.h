// SDLDeleters.h
#ifndef SDL_DELETERS_H
#define SDL_DELETERS_H

#include <SDL2/SDL.h>

struct SDLWindowDeleter
{
    void operator()(SDL_Window *window) const
    {
        if (window)
            SDL_DestroyWindow(window);
    }
};

struct SDLRendererDeleter
{
    void operator()(SDL_Renderer *renderer) const
    {
        if (renderer)
            SDL_DestroyRenderer(renderer);
    }
};

struct SDLTextureDeleter
{
    void operator()(SDL_Texture *tex) const
    {
        if (tex)
            SDL_DestroyTexture(tex);
    }
};

#endif // SDL_DELETERS_H
