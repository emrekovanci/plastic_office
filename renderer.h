#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>
#include "camera.h"

typedef struct Renderer {
    unsigned int ubo;
    bool wireframe_enabled;
} Renderer;

void renderer_init(Renderer *renderer);
void renderer_destroy(Renderer *renderer);

void renderer_begin(Renderer *renderer, int width, int height);
void renderer_end(Renderer *renderer);

void renderer_toggle_wireframe(Renderer *renderer);
void renderer_use_screen(Renderer *renderer, int width, int height);
void renderer_use_world(Renderer *renderer, const Camera *cam, int width, int height);

#endif
