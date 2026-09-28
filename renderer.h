#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>
#include "shader.h"
#include "camera.h"
#include "sprite.h"
#include "texture.h"

typedef struct Renderer {
    bool wireframe_enabled;
} Renderer;

void renderer_init(Renderer *renderer);
void renderer_toggle_wireframe(Renderer *renderer);
void renderer_clear(float r, float g, float b, float a);
void renderer_viewport_resize(int width, int height, Camera *camera, Shader *shaders[], int shader_count);

#endif
