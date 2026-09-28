#include "parallax.h"
#include "constants.h"
#include <cglm/cglm.h>

void parallax_render(Shader *shader, Camera *camera, float factor, Sprite *sprite, Texture *texture)
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
