#ifndef TILEMAP_H
#define TILEMAP_H

#include "texture.h"
#include "shader.h"
#include "constants.h"

#define TILE_INDEX(x, y) ((y) * MAP_COLS + (x))

typedef struct TilemapLayer {
    int tiles[MAP_COLS * MAP_ROWS];
    Texture texture;
    int index_count;
    unsigned int vao, vbo, ebo;
    bool visible;
} TilemapLayer;

typedef struct Tilemap {
    TilemapLayer layers[MAP_LAYERS];
} Tilemap;

bool tilemap_read(const char *path, int *map, int w, int h);
bool tilemap_init(Tilemap *out);
void tilemap_render(Tilemap *tm, Shader *s);
bool tilemap_has_collision(Tilemap *tm, int x, int y);
void tilemap_destroy(Tilemap *tm);
void tilemap_toggle_collision_visibility(Tilemap *tm);

#endif
