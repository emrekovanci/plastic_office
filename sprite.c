#include "sprite.h"

#include <glad/glad.h>
#include <cglm/cglm.h>

void sprite_init(Sprite *sprite)
{
    const float x0 = 0.0f;
    const float y0 = 0.0f;
    const float x1 = 1.0f;
    const float y1 = 1.0f;

    sprite->vertices[0] = (Vertex){ x1, y1, 0, 1, 1, 1, 1, 1 }; // top-right
    sprite->vertices[1] = (Vertex){ x1, y0, 0, 1, 1, 1, 1, 0 }; // bottom-right
    sprite->vertices[2] = (Vertex){ x0, y0, 0, 1, 1, 1, 0, 0 }; // bottom-left
    sprite->vertices[3] = (Vertex){ x0, y1, 0, 1, 1, 1, 0, 1 }; // top-left

    sprite->indices[0] = 0;
    sprite->indices[1] = 1;
    sprite->indices[2] = 3;
    
    sprite->indices[3] = 1;
    sprite->indices[4] = 2;
    sprite->indices[5] = 3;

    glGenVertexArrays(1, &sprite->vao);
    glGenBuffers(1, &sprite->vbo);
    glGenBuffers(1, &sprite->ebo);

    glBindVertexArray(sprite->vao);

    glBindBuffer(GL_ARRAY_BUFFER, sprite->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sprite->vertices), sprite->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sprite->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sprite->indices), sprite->indices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void sprite_render(Sprite *sprite, Texture *texture, Shader *shader,
                   float x, float y,
                   float size_x, float size_y,
                   float rotation,
                   float pivot_x, float pivot_y)
{
    shader_bind(shader);

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    glm_translate(model, (vec3){ x - (size_x * pivot_x), y - (size_y * pivot_y), 0.0f });

    // pivot to center transformation start
    glm_translate(model, (vec3){ size_x * pivot_x, size_y * pivot_y, 0.0f });
    glm_rotate(model, glm_rad(rotation), (vec3){ 0.0f, 0.0f, 1.0f });
    glm_translate(model, (vec3){ -size_x * pivot_x, -size_y * pivot_y, 0.0f });
    // pivot to center transformation end

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
