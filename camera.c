#include "camera.h"

void camera_reset(Camera *camera, SDL_Window *window)
{
    camera->viewport_x = 0;
    camera->viewport_y = 0;
    SDL_GetWindowSizeInPixels(window, &camera->viewport_w, &camera->viewport_h);

    camera->x = 0.0f;
    camera->y = 0.0f;
    camera->zoom = 1.0f;
    camera->zoom_target = 1.0f;
}

void camera_follow(Camera *camera, float target_x, float target_y)
{
    camera->x = target_x;
    camera->y = target_y;
}

void camera_view(Camera *camera, mat4 out)
{
    glm_mat4_identity(out);
    glm_translate(out, (vec3) { camera->viewport_w * 0.5f, camera->viewport_h * 0.5f, 0.0f });
    glm_scale(out, (vec3){ camera->zoom, camera->zoom, 1.0f });
    glm_translate(out, (vec3){ -camera->x, -camera->y, 0.0f });
}

void camera_proj(Camera *camera, mat4 out)
{
    glm_ortho(
        0, (float)camera->viewport_w,
        0, (float)camera->viewport_h,
        -1, 1, out);
}
