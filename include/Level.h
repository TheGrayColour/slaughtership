#ifndef LEVEL_H
#define LEVEL_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <memory>
#include "json.hpp"
#include "SDLDeleters.h"

using json = nlohmann::json;

// Encapsulated tile layer data.
struct TileLayer
{
    std::string name;
    int width;
    int height;
    std::vector<std::vector<int>> tiles;
    std::vector<std::vector<SDL_RendererFlip>> flipFlags;
    std::vector<std::vector<double>> rotationAngles;
};

struct Tileset
{
    int firstGid;
    std::unique_ptr<SDL_Texture, SDLTextureDeleter> texture;
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
    std::vector<TileLayer> tileLayers;    // Now using encapsulated TileLayer struct.
    std::vector<SDL_Rect> collisionTiles; // Separate collision data.

    void loadFromFile(const std::string &filename);
    bool loadTileset(const json &tilesetJson);
    Tileset *getTilesetForTileID(int tileID);

    // New: Loads a tile layer from JSON.
    TileLayer loadTileLayer(const json &layerJson, int defaultTileWidth, int defaultTileHeight);
    // New: Generates collision tiles from a tile layer.
    void generateCollisionTilesForLayer(const TileLayer &layer, int defaultTileWidth, int defaultTileHeight);
};

#endif // LEVEL_H
