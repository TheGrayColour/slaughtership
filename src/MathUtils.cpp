#include "MathUtils.h"
#include <cmath>

SDL_Rect computeTransformedRect(int x, int y, int w, int h, double rotation, SDL_Point pivot, SDL_RendererFlip flip)
{
    // Adjust y coordinate (as done previously)
    y -= 32;
    double rad = -rotation * (M_PI / 180.0);
    float pivotX = x + pivot.x;
    float pivotY = y + pivot.y;
    SDL_Point corners[4] = {
        {x, y},
        {x + w, y},
        {x, y + h},
        {x + w, y + h}};

    for (int i = 0; i < 4; i++)
    {
        if (flip & SDL_FLIP_HORIZONTAL)
            corners[i].x = 2 * pivotX - corners[i].x;
        if (flip & SDL_FLIP_VERTICAL)
            corners[i].y = 2 * pivotY - corners[i].y;
    }

    SDL_Point rotatedCorners[4];
    for (int i = 0; i < 4; i++)
    {
        float localX = corners[i].x - pivotX;
        float localY = corners[i].y - pivotY;
        rotatedCorners[i].x = pivotX + static_cast<int>(localX * cos(rad) - localY * sin(rad));
        rotatedCorners[i].y = pivotY + static_cast<int>(localX * sin(rad) + localY * cos(rad));
    }

    int minX = rotatedCorners[0].x, minY = rotatedCorners[0].y;
    int maxX = rotatedCorners[0].x, maxY = rotatedCorners[0].y;
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
