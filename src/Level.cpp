#include "Level.h"
#include <fstream>
#include <iostream>
#include <SDL2/SDL_image.h>

using json = nlohmann::json;

SDL_Rect computeTransformedRect(int x, int y, int w, int h, double rotation, SDL_Point pivot, SDL_RendererFlip flip)
{
    // Adjust
    y -= 32;

    // Convert rotation from degrees to radians (negate for correct SDL rotation)
    double rad = -rotation * (M_PI / 180.0);

    // Compute the pivot point in world space (bottom-left alignment)
    float pivotX = x + pivot.x;
    float pivotY = y + pivot.y;

    // Define the four original corners of the rectangle
    SDL_Point corners[4] = {
        {x, y},        // Top-left
        {x + w, y},    // Top-right
        {x, y + h},    // Bottom-left
        {x + w, y + h} // Bottom-right
    };

    // Apply flipping BEFORE rotation
    for (int i = 0; i < 4; i++)
    {
        if (flip & SDL_FLIP_HORIZONTAL)
        {
            corners[i].x = 2 * pivotX - corners[i].x;
        }
        if (flip & SDL_FLIP_VERTICAL)
        {
            corners[i].y = 2 * pivotY - corners[i].y;
        }
    }

    // Apply rotation around the pivot
    SDL_Point rotatedCorners[4];

    for (int i = 0; i < 4; i++)
    {
        float localX = corners[i].x - pivotX;
        float localY = corners[i].y - pivotY;

        rotatedCorners[i].x = pivotX + (localX * cos(rad) - localY * sin(rad));
        rotatedCorners[i].y = pivotY + (localX * sin(rad) + localY * cos(rad));
    }

    // Compute the final bounding box (AABB)
    int minX = rotatedCorners[0].x;
    int minY = rotatedCorners[0].y;
    int maxX = rotatedCorners[0].x;
    int maxY = rotatedCorners[0].y;

    for (int i = 1; i < 4; i++)
    {
        if (rotatedCorners[i].x < minX)
            minX = rotatedCorners[i].x;
        if (rotatedCorners[i].y < minY)
            minY = rotatedCorners[i].y;
        if (rotatedCorners[i].x > maxX)
            maxX = rotatedCorners[i].x;
        if (rotatedCorners[i].y > maxY)
            maxY = rotatedCorners[i].y;
    }

    if ((flip & SDL_FLIP_HORIZONTAL) && rotation != 90.0 && rotation != 270.0)
    {
        minX += w;
        maxX += w;
    }

    if ((flip & SDL_FLIP_VERTICAL) && rotation != 90.0 && rotation != 270.0)
    {
        minY -= h;
        maxY -= h;
    }

    if ((flip & SDL_FLIP_VERTICAL) && !(flip & SDL_FLIP_HORIZONTAL) && rotation == 90.0)
    {
        minY += w;
        maxY += w;
    }

    if ((flip & SDL_FLIP_HORIZONTAL) && !(flip & SDL_FLIP_VERTICAL) && rotation == 90.0)
    {
        minX += h;
        maxX += h;
    }

    return {minX, minY, maxX - minX, maxY - minY};
}

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
        return;
    }

    json levelData;
    file >> levelData;

    int defaultTileWidth = levelData["tilewidth"];
    int defaultTileHeight = levelData["tileheight"];

    // Load tilesets
    for (const auto &tilesetJson : levelData["tilesets"])
    {
        if (!loadTileset(tilesetJson))
        {
            //
        }
    }

    // Load tile layers
    for (const auto &layer : levelData["layers"])
    {
        if (layer["type"] == "tilelayer")
        {
            int width = layer["width"];
            int height = layer["height"];

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
                    // Adjust for additional flips
                    if (flipHorizontal && flipVertical)
                    {
                        // Case 8: D + H + V
                        // Already 90° + H, V adds vertical flip
                        rotation = 90.0;
                        flipState = SDL_FLIP_HORIZONTAL;
                    }
                    else if (flipHorizontal)
                    {
                        // Case 6: D + H
                        // 90° + H + H = 90° (H cancels), but needs V flip to match your working case
                        rotation = 270.0; // 270° = 90° counterclockwise
                        flipState = static_cast<SDL_RendererFlip>(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
                    }
                    else if (flipVertical)
                    {
                        // Case 7: D + V
                        // 90° + H, add V
                        rotation = 90.0;
                        flipState = static_cast<SDL_RendererFlip>(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
                    }
                    else
                    {
                        // Case 5: D only
                        // 90° + H
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

                // Store flipping data
                flipFlags[y][x] = flipState;
                rotationAngles[y][x] = rotation; // Store rotation angle for this tile

                // If it's the "wall" layer, add to collisionTiles
                if ((layer["name"].get<std::string>().find("wall") != std::string::npos || layer["name"] == "window") && data[i] > 0)
                {
                    Tileset *tileset = getTilesetForTileID(tileID);
                    if (!tileset)
                        continue;

                    int tileW = tileset->tileWidth;
                    int tileH = tileset->tileHeight;

                    // Base placement (same as rendering)
                    int adjustedX = x * defaultTileWidth;
                    int adjustedY = (y + 1) * defaultTileHeight - tileH;

                    // Apply pivot (same as rendering)
                    SDL_Point pivot = {0, tileH};

                    if (rotation == 270.0)
                    {
                        adjustedX += tileH;
                    }

                    if (rotation == 90.0)
                    {
                        adjustedY -= tileW;
                    }

                    SDL_Rect collisionRect = computeTransformedRect(adjustedX, adjustedY, tileW, tileH, rotation, pivot, flipState);
                    collisionTiles.push_back(collisionRect);
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
        //
        return false;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture)
    {
        //
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
    const int defaultTileWidth = 32;
    const int defaultTileHeight = 32;

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
                    //
                    continue;
                }

                int tilesPerRow = tileset->columns;
                int localID = tileID - tileset->firstGid;
                if (localID < 0)
                {
                    //
                    continue;
                }

                SDL_Rect srcRect = {
                    (localID % tilesPerRow) * tileset->tileWidth,
                    (localID / tilesPerRow) * tileset->tileHeight,
                    tileset->tileWidth,
                    tileset->tileHeight};

                SDL_RendererFlip flipState = flipLayers[i][y][x];
                double rotation = rotationLayers[i][y][x];

                int tileW = tileset->tileWidth;
                int tileH = tileset->tileHeight;

                // Base placement (bottom-left anchored to grid)
                int adjustedX = x * defaultTileWidth - cameraX;
                int adjustedY = y * defaultTileHeight - tileH - cameraY;

                if (rotation == 270.0)
                {
                    adjustedX += tileH;
                }

                if (rotation == 90.0)
                {
                    adjustedY -= tileW;
                }

                SDL_Rect destRect = {adjustedX, adjustedY, tileW, tileH};

                // Set the pivot to the bottom-left of the destination rectangle
                SDL_Point pivot = {0, tileH};

                SDL_RenderCopyEx(renderer, tileset->texture, &srcRect, &destRect, rotation, &pivot, flipState);
            }
        }
    }

    // Debugging collision tiles
    // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red for debugging
    // for (const auto &rect : collisionTiles)
    // {
    //     SDL_Rect debugRect = {rect.x - cameraX, rect.y - cameraY, rect.w, rect.h};
    //     SDL_RenderDrawRect(renderer, &debugRect); // Draw adjusted collision box
    // }
}
