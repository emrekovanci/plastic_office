#include "camera.h"
#include "constants.h"
#include "utils.h"
#include <SDL3/SDL_stdinc.h>

void camera_init(Camera *c)
{
    c->x = 0.0f;
    c->y = 0.0f;
    c->zoom = 1.0f;
    c->zoom_target = 1.0f;
}

void camera_update(Camera *c, float dt, float target_x)
{
    // zoom first
    float zoom = lerpf(c->zoom, c->zoom_target, 1.0f - expf(-10.0f * dt));
    c->zoom = zoom;

    // follow later
    const float target_y = (c->viewport_h * 0.5f) / c->zoom;
    c->x = target_x;
    c->y = target_y;
}

void camera_view(Camera *c, mat4 out)
{
    glm_mat4_identity(out);
    glm_translate(out, (vec3) { c->viewport_w * 0.5f, c->viewport_h * 0.5f, 0.0f });
    glm_scale(out, (vec3){ c->zoom, c->zoom, 1.0f });
    glm_translate(out, (vec3){ -c->x, -c->y, 0.0f });
}

void camera_proj(Camera *c, mat4 out)
{
    glm_ortho(
        0, (float)c->viewport_w,
        0, (float)c->viewport_h,
        -1, 1,
        out);
}

void camera_set_zoom(Camera *c, float dir)
{
    const float zoom_factor = (dir > 0) ? CAMERA_ZOOM_STEP : (1.0f / CAMERA_ZOOM_STEP);
    c->zoom_target = SDL_clamp(c->zoom_target * zoom_factor, camera_min_zoom(c, MAP_ROWS), camera_max_zoom(c));
}

float camera_min_zoom(const Camera *c, int map_rows)
{
    return (float)c->viewport_h / (map_rows * TILE_RENDER_SIZE);
}

float camera_max_zoom(const Camera *c)
{
    return (float)c->viewport_h / (CAMERA_MIN_VISIBLE_TILES * TILE_RENDER_SIZE);
}
