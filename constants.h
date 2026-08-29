#ifndef CONSTANTS_H
#define CONSTANTS_H

//  tiles
#define TILE_SCALE            8
#define TILE_SIZE             32
#define TILE_RENDER_SIZE      (TILE_SIZE * TILE_SCALE)

// physics
#define FIXED_TIMESTEP        (1.0f / 30.0f)

// tilemap
#define MAP_LAYERS            4
#define MAP_COLS              32
#define MAP_ROWS              16
#define COLLISION_LAYER_INDEX (MAP_LAYERS - 1)

typedef enum {
    DIRECTION_E,
    DIRECTION_W
} Direction;

#endif
