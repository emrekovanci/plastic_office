#include "utils.h"

float lerpf(float a, float b, float t)
{
    return a + (b - a) * t;
}

float signf(float val)
{
    const int a = 0.0f < val ? 1 : 0;
    const int b = val < 0.0f ? 1 : 0;

    return (float)(a - b);
}

float clampf(float val, float min, float max)
{
    if (val < min) return min;
    if (val > max) return max;
    return val;
}
