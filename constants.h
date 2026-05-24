#ifndef CONSTANTS_H
#define CONSTANTS_H

#define TILE_SCALE           8
#define TILE_SIZE            32
#define TILE_RENDER_SIZE     (TILE_SIZE * TILE_SCALE)

#define FIXED_TIMESTEP       (1.0f / 30.0f)

typedef enum {
    DIRECTION_E,
    DIRECTION_W
} Direction;

#endif
