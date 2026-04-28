#ifndef UTILS_H
#define UTILS_H

void tile_uv(int rows, int cols, int tile_id, float *u0, float *v0, float *u1, float *v1);

void swapf(float *x, float *y);
float lerpf(float a, float b, float t);

// inverts y for opengl to screen conversion
int invert_y(int y);

#endif
