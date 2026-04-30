#include "utils.h"

void tile_uv(int rows, int cols, int tile_id, float *u0, float *v0, float *u1, float *v1)
{
    const int col = tile_id % cols;
    const int row = (rows - 1) - (tile_id / cols);

    *u0 = (float)col / cols;
    *v0 = (float)row / rows;
    *u1 = *u0 + 1.0f / cols;
    *v1 = *v0 + 1.0f / rows;
}

void swapf(float *x, float *y)
{
    const float temp = *x;
    *x = *y;
    *y = temp;
}

float lerpf(float a, float b, float t)
{
    return a + (b - a) * t;
}
