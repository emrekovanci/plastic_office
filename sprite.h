#ifndef SPRITE_H
#define SPRITE_H

#include "vertex.h"
#include "texture.h"
#include "shader.h"

typedef struct Sprite Sprite;
struct Sprite {
    Vertex vertices[4];
    unsigned int indices[6];
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
};

void sprite_init(Sprite *sprite);
void sprite_render(Sprite *sprite, Texture *texture, Shader *shader, float x, float y, float size_x, float size_y, float rotation);
void sprite_destroy(Sprite *sprite);

#endif
