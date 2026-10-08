#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include <stdbool.h>
#include "camera.h"

typedef struct RenderTarget {
    unsigned int fbo, color;
    int w, h;
} RenderTarget;

bool rt_create(RenderTarget *rt, int w, int h);
void rt_destroy(RenderTarget *rt);
void rt_begin(const RenderTarget *rt);
void rt_present(const RenderTarget *rt, int win_w, int win_h);

#endif
