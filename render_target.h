#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include <stdbool.h>

typedef struct RenderTarget {
    unsigned int fbo, color;
    int w, h;
} RenderTarget;

bool rt_create(RenderTarget *rt, int w, int h);
void rt_begin(const RenderTarget *rt);
void rt_destroy(RenderTarget *rt);

typedef struct RTLayout {
    int scale, x, y;
} RTLayout;

RTLayout rt_layout(const RenderTarget *rt, int win_w, int win_h, float sub_x, float sub_y);
void rt_present(const RenderTarget *rt, int win_w, int win_h, const RTLayout *L);

#endif
