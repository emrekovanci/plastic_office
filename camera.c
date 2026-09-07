#include "camera.h"
#include "constants.h"
#include "utils.h"
#include <SDL3/SDL_stdinc.h>

void camera_init(Camera *cam)
{
    cam->viewport_w = WINDOW_WIDTH;
    cam->viewport_h = WINDOW_HEIGHT;
    cam->x = 0.0f;
    cam->y = 0.0f;
    cam->zoom = 1.0f;
    cam->zoom_target = 1.0f;
}

void camera_update(Camera *cam, float dt, float target_x)
{
    // zoom first
    float zoom = lerpf(cam->zoom, cam->zoom_target, 1.0f - expf(-10.0f * dt));
    cam->zoom = zoom;

    // follow later
    const float target_y = (cam->viewport_h * 0.5f) / cam->zoom;
    cam->x = target_x;
    cam->y = target_y;
}

void camera_view(Camera *cam, mat4 out)
{
    glm_mat4_identity(out);
    glm_translate(out, (vec3) { cam->viewport_w * 0.5f, cam->viewport_h * 0.5f, 0.0f });
    glm_scale(out, (vec3){ cam->zoom, cam->zoom, 1.0f });
    glm_translate(out, (vec3){ -cam->x, -cam->y, 0.0f });
}

void camera_proj(Camera *cam, mat4 out)
{
    glm_ortho(
        0, (float)cam->viewport_w,
        0, (float)cam->viewport_h,
        -1, 1,
        out);
}

void camera_set_zoom(Camera *cam, float dir)
{
    const float zoom_factor = (dir > 0) ? CAMERA_ZOOM_STEP : (1.0f / CAMERA_ZOOM_STEP);
    cam->zoom_target = SDL_clamp(cam->zoom_target * zoom_factor, camera_min_zoom(cam), camera_max_zoom(cam));
}

float camera_min_zoom(const Camera *cam)
{
    return (float)cam->viewport_h / (MAP_ROWS * TILE_RENDER_SIZE);
}

float camera_max_zoom(const Camera *cam)
{
    return (float)cam->viewport_h / (CAMERA_MIN_VISIBLE_TILES * TILE_RENDER_SIZE);
}
