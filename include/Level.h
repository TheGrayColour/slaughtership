#ifndef LEVEL_H
#define LEVEL_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <map>
#include "json.hpp"

struct Tileset
{
    int firstGid;
    SDL_Texture *texture;
    int tileWidth, tileHeight;
    int columns;
};

class Level
{
public:
    Level(SDL_Renderer *renderer, const std::string &filename);
    ~Level();

    void render(SDL_Renderer *renderer, int cameraX, int cameraY);
    const std::vector<SDL_Rect> &getCollisionTiles() const { return collisionTiles; }

private:
    SDL_Renderer *renderer;
    std::vector<Tileset> tilesets;
    std::vector<std::vector<std::vector<int>>> tileLayers; // Stores multiple layers
    std::vector<std::vector<std::vector<SDL_RendererFlip>>> flipLayers; // Stores flipping for each layer
    std::vector<std::vector<std::vector<double>>> rotationLayers; // Stores rotation for each layer
    std::vector<SDL_Rect> collisionTiles;                  // Store wall collisions

    void loadFromFile(const std::string &filename);
    bool loadTileset(const nlohmann::json &tilesetJson);
    Tileset *getTilesetForTileID(int tileID);
};

#endif // LEVEL_H
