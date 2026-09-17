#include <glad/glad.h>
#include "renderer.h"
#include <cglm/cglm.h>
#include "constants.h"

void renderer_init(RenderState *rs)
{
    rs->wireframe_enabled = false;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void renderer_toggle_wireframe(RenderState *rs)
{
    if (rs->wireframe_enabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        rs->wireframe_enabled = false;
        return;
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    rs->wireframe_enabled = true;
}

void renderer_clear(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void renderer_viewport_resize(int width, int height, Camera *camera, Shader *shaders[], int shader_count)
{
    camera->viewport_w = width;
    camera->viewport_h = height;
    glViewport(0, 0, width, height);

    mat4 proj;
    camera_proj(camera, proj);

    for (int i = 0; i < shader_count; ++i) {
        shader_bind(shaders[i]);
        shader_set_mat4(shaders[i], "projection", (float*)proj);
    }
}

void renderer_draw_parallax_layer(Shader *shader, Camera *camera, float factor, Sprite *sprite, Texture *texture)
{
    // build a standalone view matrix for this layer (independent of the camera's actual view matrix)
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
