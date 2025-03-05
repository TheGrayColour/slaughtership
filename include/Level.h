#ifndef LEVEL_H
#define LEVEL_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include "json.hpp"

class Level
{
public:
    Level(SDL_Renderer *renderer, const std::string &filename);
    ~Level();

    void render();

private:
    SDL_Renderer *renderer;
    std::vector<std::vector<int>> tileMap;
    SDL_Texture *tilesetTexture;

    void loadFromFile(const std::string &filename);
    bool loadTileset(const std::string &path);
};

#endif // LEVEL_H
