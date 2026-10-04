#include <glad/glad.h>
#include "renderer.h"

#include <cglm/cglm.h>
#include "constants.h"

static void upload_matrices(Renderer *renderer, mat4 view, mat4 proj)
{
    glBindBuffer(GL_UNIFORM_BUFFER, renderer->ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), view);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), proj);
}

void renderer_init(Renderer *renderer)
{
    renderer->wireframe_enabled = false;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ubo holds the vp matrices
    glGenBuffers(1, &renderer->ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, renderer->ubo);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(mat4), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, UBO_BINDING_MATRICES, renderer->ubo);
}

void renderer_destroy(Renderer *renderer)
{
    glDeleteBuffers(1, &renderer->ubo);
}

void renderer_begin(Renderer *renderer, int width, int height)
{
    glViewport(0, 0, width, height);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void renderer_end(Renderer *renderer)
{

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

void renderer_use_screen(Renderer *renderer, int width, int height)
{
    mat4 view, proj;
    glm_mat4_identity(view);
    glm_ortho(0.0f, (float)width, 0.0f, (float)height, -1, 1, proj);
    upload_matrices(renderer, view, proj);
}

void renderer_use_world(Renderer *renderer, const Camera *cam, int width, int height)
{
    mat4 view, proj;
    camera_view(cam, view);
    camera_proj(cam, width, height, proj);
    upload_matrices(renderer, view, proj);
}
