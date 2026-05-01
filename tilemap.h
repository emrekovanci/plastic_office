#ifndef TILEMAP_H
#define TILEMAP_H

#include "constants.h"
#include "vertex.h"
#include "texture.h"
#include "shader.h"

typedef struct TilemapLayer TilemapLayer;
struct TilemapLayer {
    Vertex vertices[MAP_COLS * MAP_ROWS * 4];
    unsigned int indices[MAP_COLS * MAP_ROWS * 6];
    int vertex_count;
    int index_count;
    unsigned int vao, vbo, ebo;
    Texture texture;
};

typedef struct Tilemap Tilemap;
struct Tilemap {
    TilemapLayer layers[MAP_LAYERS];
    int collision[MAP_ROWS][MAP_COLS];
};

bool tilemap_read(const char *path, int out[MAP_ROWS][MAP_COLS]);
bool tilemap_build(Tilemap *out);
void tilemap_render(Tilemap *tilemap, Shader *shader);
bool tilemap_has_collision(Tilemap *tilemap, int x, int y);
void tilemap_destroy(Tilemap *tilemap);

#endif
