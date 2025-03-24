#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <SDL2/SDL.h>

// Computes a transformed rectangle given position, size, rotation (in degrees),
// a pivot point, and a flip state.
SDL_Rect computeTransformedRect(int x, int y, int w, int h, double rotation, SDL_Point pivot, SDL_RendererFlip flip);

#endif // MATH_UTILS_H
