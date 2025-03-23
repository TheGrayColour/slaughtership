#ifndef LEVEL_H
#define LEVEL_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <memory>
#include "json.hpp"
#include "SDLDeleters.h" // For our custom SDL deleters

// Structure representing a tileset.
struct Tileset
{
    int firstGid;
    // Texture wrapped in a unique_ptr with custom deleter.
    std::unique_ptr<SDL_Texture, SDLTextureDeleter> texture;
    int tileWidth, tileHeight;
    int columns;
};

class Level
{
public:
    Level(SDL_Renderer *renderer, const std::string &filename);
    ~Level();

    // Renders the level using the provided renderer and camera offset.
    void render(SDL_Renderer *renderer, int cameraX, int cameraY);
    // Provides read-only access to the collision tiles.
    const std::vector<SDL_Rect> &getCollisionTiles() const { return collisionTiles; }

private:
    // Renderer is not owned by Level.
    SDL_Renderer *renderer;
    std::vector<Tileset> tilesets;
    // 3D vectors: layers -> rows -> columns.
    std::vector<std::vector<std::vector<int>>> tileLayers;
    std::vector<std::vector<std::vector<SDL_RendererFlip>>> flipLayers;
    std::vector<std::vector<std::vector<double>>> rotationLayers;
    std::vector<SDL_Rect> collisionTiles; // Holds collision rectangles.

    // Loads level data from a JSON file.
    void loadFromFile(const std::string &filename);
    // Loads a tileset from JSON data.
    bool loadTileset(const nlohmann::json &tilesetJson);
    // Returns the tileset corresponding to a given tileID.
    Tileset *getTilesetForTileID(int tileID);
};

#endif // LEVEL_H