#ifndef DIRECTION_H
#define DIRECTION_H

typedef enum {
    DIRECTION_N       = 0x00, // n
    DIRECTION_NE      = 0x01, // ne
    DIRECTION_E       = 0x02, // e
    DIRECTION_SE      = 0x03, // se
    DIRECTION_S       = 0x04, // s
    DIRECTION_SW      = 0x05, // sw
    DIRECTION_W       = 0x06, // w
    DIRECTION_NW      = 0x07, // nw
    DIRECTION_MASK    = 0x7,
    DIRECTION_RUNNING = 0x80,
    DIRECTION_NONE    = 0xED
} Direction;

extern const int DIR_DELTA_X[8];
extern const int DIR_DELTA_Y[8];

int get_mouse_direction(int x1, int y1, int to_x, int to_y, int current_facing);

#endif
