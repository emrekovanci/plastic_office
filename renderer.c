#include <glad/glad.h>
#include <cglm/cglm.h>
#include "renderer.h"
#include "constants.h"

void renderer_init(RenderState *rs)
{
    rs->blend_enabled = true;
    rs->wireframe_enabled = false;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void renderer_set_blend(RenderState *rs, bool enabled)
{
    if (rs->blend_enabled == enabled)
        return;

    rs->blend_enabled = enabled;
    if (enabled)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
}

void renderer_set_wireframe(RenderState *rs, bool enabled)
{
    if (rs->wireframe_enabled == enabled)
        return;

    rs->wireframe_enabled = enabled;
    glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL);
}

void renderer_clear(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void renderer_draw_parallax_layer(Shader *shader, Camera *camera, float factor, Sprite *sprite, Texture *texture)
{
    mat4 view;
    glm_mat4_identity(view);
    vec3 translation = { -camera->x * factor, 0.0f, 0.0f };
    glm_translate(view, translation);
    shader_set_mat4(shader, "view", (float*)view);

    sprite_render(sprite,
                  texture,
                  shader,
                  0.0f, 0.0f,
                  (float)texture->width * TILE_SCALE,
                  (float)texture->height * TILE_SCALE,
                  0.0f,
                  0.0f, 0.0f);
}
