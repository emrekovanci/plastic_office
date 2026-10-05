#ifndef CONSTANTS_H
#define CONSTANTS_H

#define VIRT_W                   320
#define VIRT_H                   180

// ------
// window
// ------
#define WINDOW_WIDTH             1920
#define WINDOW_HEIGHT            1080
#define WINDOW_GL_MAJOR_VERSION  4
#define WINDOW_GL_MINOR_VERSION  3

// --------
// renderer
// --------
#define UBO_BINDING_MATRICES     0

// --
// ui
// --
#define UI_REFERENCE_HEIGHT      1080.0f

// -----
// tiles
// -----
#define TILE_SCALE               1
#define TILE_SIZE                8
#define TILE_RENDER_SIZE         (TILE_SIZE * TILE_SCALE)

// -------
// physics
// -------
#define FIXED_TIMESTEP           (1.0f / 30.0f)

// -------
// tilemap
// -------
#define MAP_LAYERS               3
#define MAP_COLS                 40
#define MAP_ROWS                 23
#define COLLISION_LAYER_INDEX    (MAP_LAYERS - 1)

// ------
// camera
// ------
#define CAMERA_MIN_TILES         8.0f              // closest
#define CAMERA_MAX_TILES         ((float)MAP_ROWS) // far away (whole map)

// ------
// others
// ------
typedef enum {
    DIRECTION_E,
    DIRECTION_W
} Direction;

#endif
