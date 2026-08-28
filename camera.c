#include "camera.h"
#include "constants.h"
#include "utils.h"

void camera_init(Camera *camera)
{
    camera->x = 0.0f;
    camera->y = 0.0f;
    camera->zoom = 1.0f;
    camera->zoom_target = 1.0f;
}

void camera_update(Camera *camera, float dt, float target_x)
{
    // zoom first
    float zoom = lerpf(camera->zoom, camera->zoom_target, 1.0f - expf(-10.0f * dt));
    camera->zoom = zoom;

    // follow later
    const float target_y = (camera->viewport_h * 0.5f) / camera->zoom;
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

float camera_min_zoom(const Camera *camera, int map_rows)
{
    return (float)camera->viewport_h / (map_rows * TILE_RENDER_SIZE);
}

float camera_max_zoom(const Camera *camera)
{
    const float MIN_VISIBLE_TILES = 6.0f;
    return (float)camera->viewport_h / (MIN_VISIBLE_TILES * TILE_RENDER_SIZE);
}
