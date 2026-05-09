#ifndef COLOR_H
#define COLOR_H

#include <SDL3/SDL_pixels.h>

typedef SDL_Color Color;

#define WHITE (Color){ 255, 255, 255, 255 }
#define BLACK (Color){ 0,   0,   0,   255 }
#define RED   (Color){ 255, 0,   0,   255 }
#define GREEN (Color){ 0,   255, 0,   255 }
#define BLUE  (Color){ 0,   0,   255, 255 }

#endif
