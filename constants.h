#ifndef CONSTANTS_H
#define CONSTANTS_H

#define VIRT_W                   320
#define VIRT_H                   180

// ------
// window
// ------
#define WINDOW_WIDTH             1280
#define WINDOW_HEIGHT            800
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
#define TILE_SIZE                8

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
// others
// ------
typedef enum {
    DIRECTION_E,
    DIRECTION_W
} Direction;

#endif
