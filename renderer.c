#include <glad/glad.h>
#include "renderer.h"

#include <cglm/cglm.h>

void renderer_init(Renderer *renderer)
{
    renderer->wireframe_enabled = false;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void renderer_clear(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void renderer_toggle_wireframe(Renderer *renderer)
{
    if (renderer->wireframe_enabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        renderer->wireframe_enabled = false;
        return;
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    renderer->wireframe_enabled = true;
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
