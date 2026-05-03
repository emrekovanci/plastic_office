#ifndef TILEMAP_H
#define TILEMAP_H

#include "vertex.h"
#include "texture.h"
#include "shader.h"

#define MAP_LAYERS 4
#define MAP_COLS   32
#define MAP_ROWS   16

typedef struct TilemapLayer TilemapLayer;
struct TilemapLayer {
    Vertex vertices[MAP_COLS * MAP_ROWS * 4];
    unsigned int indices[MAP_COLS * MAP_ROWS * 6];
    Texture texture;
    int vertex_count;
    int index_count;
    unsigned int vao, vbo, ebo;
    bool visible;
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
