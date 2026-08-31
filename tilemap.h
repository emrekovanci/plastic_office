#ifndef TILEMAP_H
#define TILEMAP_H

#include "vertex.h"
#include "texture.h"
#include "shader.h"
#include "constants.h"

#define TILE_INDEX(x, y) ((y) * MAP_COLS + (x))

typedef struct TilemapLayerDef TilemapLayerDef;
struct TilemapLayerDef {
    const char *csv;
    const char *texture;
};

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
    int collision[MAP_COLS * MAP_ROWS];
};

bool tilemap_read(const char *path, int *map, int w, int h);
bool tilemap_build(Tilemap *out);
void tilemap_render(Tilemap *t, Shader *s);
bool tilemap_has_collision(Tilemap *t, int x, int y);
void tilemap_destroy(Tilemap *t);
void tilemap_toggle_collision_visibility(Tilemap *t);

#endif
