#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>
#include "shader.h"
#include "camera.h"
#include "sprite.h"
#include "texture.h"

typedef struct RenderState RenderState;
struct RenderState {
    bool blend_enabled;
    bool wireframe_enabled;
};

void renderer_init(RenderState *rs);
void renderer_toggle_wireframe(RenderState *rs);
void renderer_clear(float r, float g, float b, float a);

void renderer_viewport_resize(int width, int height, Camera *camera, Shader *shaders[], int shader_count);

void renderer_draw_parallax_layer(Shader *shader, Camera *camera, float factor, Sprite *sprite, Texture *texture);

#endif
