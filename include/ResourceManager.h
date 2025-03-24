#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <unordered_map>

class ResourceManager
{
public:
    static SDL_Texture *loadTexture(SDL_Renderer *renderer, const std::string &path);
    static void clear();

private:
    static std::unordered_map<std::string, SDL_Texture *> textureCache;
};

#endif // RESOURCE_MANAGER_H
