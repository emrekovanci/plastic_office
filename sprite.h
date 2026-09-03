#ifndef SPRITE_H
#define SPRITE_H

#include "texture.h"
#include "shader.h"

typedef struct Sprite Sprite;
struct Sprite {
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
};

void sprite_create(Sprite *sprite);

// ================== Pivots ==================
// 0.5f, 0.5f = center        (general)
// 0.5f, 0.0f = bottom center (characters)
// 0.0f, 0.0f = bottom left
// 1.0f, 1.0f = top right
// 0.0f, 1.0f = top left
// ============================================
void sprite_render(Sprite *sprite, Texture *texture, Shader *shader,
                   float x, float y,
                   float size_x, float size_y,
                   float rotation,
                   float pivot_x, float pivot_y);

void sprite_destroy(Sprite *sprite);

#endif
