#include <glad/glad.h>
#include "sprite.h"

#include <cglm/cglm.h>
#include "vertex.h"

void sprite_create(Sprite *sprite)
{
    const float x0 = 0.0f;
    const float y0 = 0.0f;
    const float x1 = 1.0f;
    const float y1 = 1.0f;

    // counter-clockwise vertices and indices order
    const Vertex vertices[4] = {
        { x0, y0, 0, 1, 1, 1, 0, 0 }, // bottom-left
        { x1, y0, 0, 1, 1, 1, 1, 0 }, // bottom-right
        { x1, y1, 0, 1, 1, 1, 1, 1 }, // top-right
        { x0, y1, 0, 1, 1, 1, 0, 1 }  // top-left
    };

    const unsigned int indices[6] = {
        0, 1, 2,
        0, 2, 3
    };

    glGenVertexArrays(1, &sprite->vao);
    glGenBuffers(1, &sprite->vbo);
    glGenBuffers(1, &sprite->ebo);
    glBindVertexArray(sprite->vao);

    glBindBuffer(GL_ARRAY_BUFFER, sprite->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sprite->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void sprite_render(Sprite *sprite, Texture *texture, Shader *shader,
                   float x, float y,
                   float size_x, float size_y,
                   float rotation_deg,
                   float pivot_x, float pivot_y)
{
    mat4 model = GLM_MAT4_IDENTITY_INIT;

    // translate
    glm_translate(model, (vec3){ x - (size_x * pivot_x), y - (size_y * pivot_y), 0.0f });
    
    // rotation around pivot
    glm_translate(model, (vec3){ pivot_x * size_x, pivot_y * size_y, 0.0f });
    glm_rotate(model, glm_rad(rotation_deg), (vec3){ 0.0f, 0.0f, 1.0f });
    glm_translate(model, (vec3){ -pivot_x * size_x, -pivot_y * size_y, 0.0f });

    // scale
    glm_scale(model, (vec3){ size_x, size_y, 1.0f });

    shader_set_mat4(shader, "model", (float*)model);
    texture_bind(texture, 0);
    glBindVertexArray(sprite->vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void sprite_destroy(Sprite *sprite)
{
    glDeleteVertexArrays(1, &sprite->vao);
    glDeleteBuffers(1, &sprite->vbo);
    glDeleteBuffers(1, &sprite->ebo);
}
