#ifndef SPRITE_H
#define SPRITE_H

#include "texture.h"
#include "shader.h"

typedef struct Sprite {
    unsigned int vao, vbo, ebo;
} Sprite;

void sprite_create(Sprite *sprite);

// ============================================================
//                     Pivot Points
// ============================================================
//
//   (0,1) TOP_LEFT       TOP_CENTER        TOP_RIGHT (1,1)
//         o-------------------o-------------------o
//         |                   |                   |
//         |                   |                   |
//         |                   |                   |
//  LEFT_CENTER                |               RIGHT_CENTER
//  (0,0.5) o- - - - - - - - - o- - - - - - - - - -o (1,0.5)
//         |               CENTER (0.5,0.5)        |
//         |                   |                   |
//         |                   |                   |
//         o-------------------o-------------------o
//   (0,0) BOTTOM_LEFT   BOTTOM_CENTER    BOTTOM_RIGHT (1,0)
//
//   x axis: left -> right   (0.0 -> 1.0)
//   y axis: bottom -> top   (0.0 -> 1.0)
//
//   NOTES:
//   - Use "Bottom Center" pivot for characters
// ============================================================
void sprite_render(Sprite *sprite, Texture *texture, Shader *shader,
                   float x, float y,
                   float size_x, float size_y,
                   float rotation_deg,
                   float pivot_x, float pivot_y);

void sprite_destroy(Sprite *sprite);

#endif
