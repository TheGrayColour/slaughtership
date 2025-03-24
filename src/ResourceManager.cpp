#include "ResourceManager.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

std::unordered_map<std::string, SDL_Texture *> ResourceManager::textureCache;

SDL_Texture *ResourceManager::loadTexture(SDL_Renderer *renderer, const std::string &path)
{
    auto it = textureCache.find(path);
    if (it != textureCache.end())
        return it->second;
    SDL_Texture *tex = IMG_LoadTexture(renderer, path.c_str());
    if (!tex)
    {
        SDL_Log("Failed to load texture: %s, SDL_Error: %s", path.c_str(), SDL_GetError());
        return nullptr;
    }
    textureCache[path] = tex;
    return tex;
}

void ResourceManager::clear()
{
    for (auto &pair : textureCache)
    {
        SDL_DestroyTexture(pair.second);
    }
    textureCache.clear();
}
