#include "utils.h"

float lerpf(float a, float b, float t)
{
    return a + (b - a) * t;
}
