#include "Level.h"
#include <fstream>
#include <iostream>
#include <SDL2/SDL_image.h>

using json = nlohmann::json;

Level::Level(SDL_Renderer *renderer, const std::string &filename) : renderer(renderer)
{
    loadFromFile(filename);
}

Level::~Level()
{
    for (auto &tileset : tilesets)
    {
        SDL_DestroyTexture(tileset.texture);
    }
}

void Level::loadFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "Failed to open " << filename << std::endl;
        return;
    }

    json levelData;
    file >> levelData;

    int width = levelData["width"];
    int height = levelData["height"];

    // Load tilesets
    for (const auto &tilesetJson : levelData["tilesets"])
    {
        if (!loadTileset(tilesetJson))
        {
            std::cerr << "Failed to load tileset: " << tilesetJson["image"] << std::endl;
        }
    }

    // Load tile layers
    for (const auto &layer : levelData["layers"])
    {
        if (layer["type"] == "tilelayer")
        {
            std::vector<std::vector<int>> layerData(height, std::vector<int>(width));
            std::vector<std::vector<SDL_RendererFlip>> flipFlags(height, std::vector<SDL_RendererFlip>(width, SDL_FLIP_NONE));
            std::vector<std::vector<double>> rotationAngles(height, std::vector<double>(width, 0.0)); // Initialize rotation angles
            auto &data = layer["data"];

            // Mask out flipping flags (Tiled uses bits 30-32 for flags)
            const int FLIP_MASK = 0x1FFFFFFF; // 29-bit mask to extract the real tile ID

            for (size_t i = 0; i < data.size(); i++)
            {
                int x = i % width;
                int y = i / width;
                int rawTileID = data[i].get<int>();
                int tileID = rawTileID & FLIP_MASK; // Extract real tile ID

                bool flipHorizontal = (rawTileID & 0x80000000) != 0;
                bool flipVertical = (rawTileID & 0x40000000) != 0;
                bool flipDiagonal = (rawTileID & 0x20000000) != 0;

                layerData[y][x] = tileID; // Store real tile ID

                // Handle flipping and rotation
                SDL_RendererFlip flipState = SDL_FLIP_NONE;
                double rotation = 0.0;

                if (flipDiagonal)
                {
                    // Diagonal flip = swap X/Y axis, effectively rotating the tile 90°
                    rotation = 90.0;
                    std::swap(flipHorizontal, flipVertical);
                }

                if (flipHorizontal)
                    flipState = SDL_FLIP_HORIZONTAL;
                if (flipVertical)
                    flipState = static_cast<SDL_RendererFlip>(flipState | SDL_FLIP_VERTICAL);

                // Store flipping data
                flipFlags[y][x] = flipState;
                rotationAngles[y][x] = rotation; // Store rotation angle for this tile

                // If it's the "wall" layer, add to collisionTiles
                if (layer["name"] == "wall" && data[i] > 0)
                {
                    collisionTiles.push_back({x * 64, y * 64, 64, 64});
                }
            }

            tileLayers.push_back(layerData);
            flipLayers.push_back(flipFlags);
            rotationLayers.push_back(rotationAngles);
        }
    }
}

bool Level::loadTileset(const json &tilesetJson)
{
    std::string imagePath = "assets/map/" + tilesetJson["image"].get<std::string>();
    SDL_Surface *surface = IMG_Load(imagePath.c_str());
    if (!surface)
    {
        std::cerr << "Failed to load tileset: " << imagePath << " SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture)
    {
        std::cerr << "Failed to create texture from: " << imagePath << " SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    Tileset tileset;
    tileset.firstGid = tilesetJson["firstgid"];
    tileset.texture = texture;
    tileset.tileWidth = tilesetJson["tilewidth"];
    tileset.tileHeight = tilesetJson["tileheight"];
    tileset.columns = tilesetJson["columns"];

    tilesets.push_back(tileset);
    return true;
}

Tileset *Level::getTilesetForTileID(int tileID)
{
    for (auto it = tilesets.rbegin(); it != tilesets.rend(); ++it)
    {
        if (tileID >= it->firstGid)
        {
            return &(*it);
        }
    }
    return nullptr; // Tile ID is invalid or missing
}

void Level::render(SDL_Renderer *renderer, int cameraX, int cameraY)
{
    for (size_t i = 0; i < tileLayers.size(); i++)
    {
        for (size_t y = 0; y < tileLayers[i].size(); y++)
        {
            for (size_t x = 0; x < tileLayers[i][y].size(); x++)
            {
                int tileID = tileLayers[i][y][x];
                if (tileID == 0)
                    continue;

                Tileset *tileset = getTilesetForTileID(tileID);
                if (!tileset)
                {
                    std::cerr << "No tileset found for tileID: " << tileID << std::endl;
                    continue;
                }

                int tilesPerRow = tileset->columns;
                int localID = tileID - tileset->firstGid;
                if (localID < 0)
                {
                    std::cerr << "Invalid localID: " << localID << " for tileID: " << tileID << std::endl;
                    continue;
                }

                SDL_Rect srcRect = {
                    (localID % tilesPerRow) * tileset->tileWidth,
                    (localID / tilesPerRow) * tileset->tileHeight,
                    tileset->tileWidth,
                    tileset->tileHeight};

                SDL_Rect destRect = {
                    static_cast<int>(x * tileset->tileWidth - cameraX),
                    static_cast<int>(y * tileset->tileHeight - cameraY),
                    tileset->tileWidth,
                    tileset->tileHeight};

                SDL_RenderCopyEx(renderer, tileset->texture, &srcRect, &destRect,
                                 rotationLayers[i][y][x], nullptr, flipLayers[i][y][x]);
            }
        }
    }
}
