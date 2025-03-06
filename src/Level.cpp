#include "Level.h"
#include <fstream>
#include <iostream>
#include <SDL2/SDL_image.h>

using json = nlohmann::json;

Level::Level(SDL_Renderer *renderer, const std::string &filename)
    : renderer(renderer), tilesetTexture(nullptr)
{
    loadFromFile(filename);
}

Level::~Level()
{
    SDL_DestroyTexture(tilesetTexture);
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
    tileMap.resize(height, std::vector<int>(width));

    auto &layers = levelData["layers"];
    for (const auto &layer : layers)
    {
        if (layer["type"] == "tilelayer")
        {
            const auto &data = layer["data"];
            for (size_t i = 0; i < data.size(); i++)
            {
                int x = static_cast<int>(i % width);
                int y = static_cast<int>(i / width);
                tileMap[y][x] = data[i];
            }
        }
    }

    if (!loadTileset("assets/map/tileset.png"))
    {
        std::cerr << "Error loading tileset texture" << std::endl;
    }
}

bool Level::loadTileset(const std::string &path)
{
    SDL_Surface *surface = IMG_Load(path.c_str());
    if (!surface)
    {
        std::cerr << "Failed to load tileset: " << IMG_GetError() << std::endl;
        return false;
    }

    tilesetTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_SetTextureBlendMode(tilesetTexture, SDL_BLENDMODE_NONE);

    SDL_FreeSurface(surface);
    return tilesetTexture != nullptr;
}

void Level::render(SDL_Renderer *renderer, int cameraX, int cameraY)
{

    if (!tilesetTexture)
        return;

    int tileSize = 64;
    int tilesetWidth, tilesetHeight;
    SDL_QueryTexture(tilesetTexture, nullptr, nullptr, &tilesetWidth, &tilesetHeight);
    int tilesPerRow = tilesetWidth / tileSize; // Use actual tileset width

    for (size_t y = 0; y < tileMap.size(); y++)
    {
        for (size_t x = 0; x < tileMap[y].size(); x++)
        {
            int tileID = tileMap[y][x];
            if (tileID == 0)
                continue;

            SDL_Rect srcRect = {((tileID - 1) % tilesPerRow) * 64, ((tileID - 1) / tilesPerRow) * 64, tileSize, tileSize};
            SDL_Rect destRect = {static_cast<int>(x * tileSize) - cameraX, static_cast<int>(y * tileSize) - cameraY, tileSize, tileSize};

            SDL_RenderCopy(renderer, tilesetTexture, &srcRect, &destRect);
        }
    }
}
