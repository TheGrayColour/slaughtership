#include "Level.h"
#include "MathUtils.h" // Use our dedicated math utility.
#include <fstream>
#include <iostream>
#include <SDL2/SDL_image.h>
#include <cmath>

Level::Level(SDL_Renderer *renderer, const std::string &filename) : renderer(renderer)
{
    loadFromFile(filename);
}

Level::~Level()
{
    // Smart pointers clean up textures automatically.
}

void Level::loadFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "Failed to open level file: " << filename << std::endl;
        return;
    }

    json levelData;
    file >> levelData;

    int defaultTileWidth = levelData["tilewidth"];
    int defaultTileHeight = levelData["tileheight"];

    // Load tilesets.
    for (const auto &tilesetJson : levelData["tilesets"])
    {
        if (!loadTileset(tilesetJson))
        {
            // Error already logged.
        }
    }

    // Load tile layers.
    for (const auto &layer : levelData["layers"])
    {
        if (layer["type"] == "tilelayer")
        {
            // Use the new function to encapsulate layer data.
            TileLayer tileLayer = loadTileLayer(layer, defaultTileWidth, defaultTileHeight);
            tileLayers.push_back(tileLayer);

            // If the layer represents collision (e.g., walls/windows), generate collision tiles.
            std::string layerName = layer["name"].get<std::string>();
            if (layerName.find("wall") != std::string::npos || layerName == "window")
            {
                generateCollisionTilesForLayer(tileLayer, defaultTileWidth, defaultTileHeight);
            }
        }
    }
}

bool Level::loadTileset(const json &tilesetJson)
{
    std::string imagePath = "assets/map/" + tilesetJson["image"].get<std::string>();
    SDL_Surface *surface = IMG_Load(imagePath.c_str());
    if (!surface)
    {
        std::cerr << "Failed to load surface: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_Texture *rawTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!rawTexture)
    {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
        return false;
    }

    Tileset tileset;
    tileset.firstGid = tilesetJson["firstgid"];
    tileset.texture.reset(rawTexture);
    tileset.tileWidth = tilesetJson["tilewidth"];
    tileset.tileHeight = tilesetJson["tileheight"];
    tileset.columns = tilesetJson["columns"];

    tilesets.push_back(std::move(tileset));
    return true;
}

Tileset *Level::getTilesetForTileID(int tileID)
{
    for (auto it = tilesets.rbegin(); it != tilesets.rend(); ++it)
    {
        if (tileID >= it->firstGid)
            return &(*it);
    }
    return nullptr;
}

TileLayer Level::loadTileLayer(const json &layerJson, int defaultTileWidth, int defaultTileHeight)
{
    TileLayer layer;
    layer.name = layerJson["name"].get<std::string>();
    layer.width = layerJson["width"];
    layer.height = layerJson["height"];

    // Initialize 2D vectors.
    layer.tiles.resize(layer.height, std::vector<int>(layer.width, 0));
    layer.flipFlags.resize(layer.height, std::vector<SDL_RendererFlip>(layer.width, SDL_FLIP_NONE));
    layer.rotationAngles.resize(layer.height, std::vector<double>(layer.width, 0.0));

    auto &data = layerJson["data"];
    const int FLIP_MASK = 0x1FFFFFFF;

    for (size_t i = 0; i < data.size(); i++)
    {
        int x = i % layer.width;
        int y = i / layer.width;
        int rawTileID = data[i].get<int>();
        int tileID = rawTileID & FLIP_MASK;

        bool flipHorizontal = (rawTileID & 0x80000000) != 0;
        bool flipVertical = (rawTileID & 0x40000000) != 0;
        bool flipDiagonal = (rawTileID & 0x20000000) != 0;

        layer.tiles[y][x] = tileID;

        SDL_RendererFlip flipState = SDL_FLIP_NONE;
        double rotation = 0.0;

        if (flipDiagonal)
        {
            if (flipHorizontal && flipVertical)
            {
                rotation = 90.0;
                flipState = SDL_FLIP_HORIZONTAL;
            }
            else if (flipHorizontal)
            {
                rotation = 270.0;
                flipState = static_cast<SDL_RendererFlip>(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
            }
            else if (flipVertical)
            {
                rotation = 90.0;
                flipState = static_cast<SDL_RendererFlip>(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
            }
            else
            {
                rotation = 90.0;
                flipState = SDL_FLIP_VERTICAL;
            }
        }
        else
        {
            rotation = 0.0;
            if (flipHorizontal)
                flipState = SDL_FLIP_HORIZONTAL;
            if (flipVertical)
                flipState = static_cast<SDL_RendererFlip>(flipState | SDL_FLIP_VERTICAL);
        }

        layer.flipFlags[y][x] = flipState;
        layer.rotationAngles[y][x] = rotation;
    }

    return layer;
}

void Level::generateCollisionTilesForLayer(const TileLayer &layer, int defaultTileWidth, int defaultTileHeight)
{
    // For each tile in the layer, if the tile is non-zero, generate a collision rectangle.
    for (int y = 0; y < layer.height; y++)
    {
        for (int x = 0; x < layer.width; x++)
        {
            int tileID = layer.tiles[y][x];
            if (tileID == 0)
                continue;

            Tileset *tileset = getTilesetForTileID(tileID);
            if (!tileset)
                continue;

            int tileW = tileset->tileWidth;
            int tileH = tileset->tileHeight;
            int adjustedX = x * defaultTileWidth;
            int adjustedY = (y + 1) * defaultTileHeight - tileH;
            SDL_Point pivot = {0, tileH};

            double rotation = layer.rotationAngles[y][x];
            SDL_RendererFlip flipState = layer.flipFlags[y][x];

            if (rotation == 270.0)
            {
                adjustedX += tileH;
            }

            if (rotation == 90.0)
            {
                adjustedY -= tileW;
            }

            // Use our dedicated math utility for transformed rect.
            SDL_Rect collisionRect = computeTransformedRect(adjustedX, adjustedY, tileW, tileH, rotation, pivot, flipState);
            collisionTiles.push_back(collisionRect);
        }
    }
}

void Level::render(SDL_Renderer *renderer, int cameraX, int cameraY)
{
    const int defaultTileWidth = 32;
    const int defaultTileHeight = 32;

    for (const auto &layer : tileLayers)
    {
        for (int y = 0; y < layer.height; y++)
        {
            for (int x = 0; x < layer.width; x++)
            {
                int tileID = layer.tiles[y][x];
                if (tileID == 0)
                    continue;

                Tileset *tileset = getTilesetForTileID(tileID);
                if (!tileset)
                    continue;

                int tilesPerRow = tileset->columns;
                int localID = tileID - tileset->firstGid;
                if (localID < 0)
                    continue;

                SDL_Rect srcRect = {
                    (localID % tilesPerRow) * tileset->tileWidth,
                    (localID / tilesPerRow) * tileset->tileHeight,
                    tileset->tileWidth,
                    tileset->tileHeight};

                SDL_RendererFlip flipState = layer.flipFlags[y][x];
                double rotation = layer.rotationAngles[y][x];

                int tileW = tileset->tileWidth;
                int tileH = tileset->tileHeight;
                int adjustedX = x * defaultTileWidth - cameraX;
                int adjustedY = y * defaultTileHeight - tileH - cameraY;

                if (rotation == 270.0)
                    adjustedX += tileH;
                if (rotation == 90.0)
                    adjustedY -= tileW;

                SDL_Rect destRect = {adjustedX, adjustedY, tileW, tileH};
                SDL_Point pivot = {0, tileH};

                SDL_RenderCopyEx(renderer, tileset->texture.get(), &srcRect, &destRect, rotation, &pivot, flipState);
            }
        }
    }
}
