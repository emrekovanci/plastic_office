#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include <stdbool.h>

#define RT_MARGIN 1   // texels, each side

// o---------------------------------------------------------------o
// |                     WINDOW  (win_w x win_h)                   |
// |                                                               |
// |     o---------------------------------------------------o     |
// |     |                 VIRTUAL (322 x 182)               |     |
// |     |                                                   |     |
// |     |     o---------------------------------------o     |     |
// |     |     |           LOGICAL (320 x 180)         |     |     |
// |     |     |                                       |     |     |
// |     |     |                                       |     |     |
// |     |     |                                       |     |     |
// |     |     o---------------------------------------o     |     |
// |     |                                                   |     |
// |     |                                                   |     |
// |     o---------------------------------------------------o     |
// |                                                               |
// |                                                               |
// o---------------------------------------------------------------o
     
typedef struct RenderTarget {
    unsigned int fbo, color;
    int w, h;
} RenderTarget;

typedef struct RTLayout {
    int scale;
    int x, y;
} RTLayout;

bool rt_create(RenderTarget *rt, int w, int h);
void rt_destroy(RenderTarget *rt);

void rt_begin(const RenderTarget *rt);

RTLayout rt_layout(const RenderTarget *rt, int win_w, int win_h, float sub_x, float sub_y);
void rt_present(const RenderTarget *rt, int win_w, int win_h, const RTLayout *L);

#endif
