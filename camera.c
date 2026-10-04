#include "camera.h"
#include "constants.h"
#include "utils.h"

#define CAMERA_MIN_TILES 8.0f              // closest
#define CAMERA_MAX_TILES ((float)MAP_ROWS) // far away (whole map)

static float camera_world_h(const Camera *cam)
{
    return cam->vt * TILE_RENDER_SIZE;
}

void camera_init(Camera *cam)
{
    *cam = (Camera) {
        .x = 0.0f,
        .y = 0.0f,
        .vt = CAMERA_MAX_TILES,
        .vt_target = CAMERA_MAX_TILES
    };
}

void camera_update(Camera *cam, float dt, float target_x, float target_y)
{
    cam->vt = lerpf(cam->vt, cam->vt_target, 1.0f - expf(-10.0f * dt));

    if (fabsf(cam->vt - cam->vt_target) < 0.001f)
        cam->vt = cam->vt_target;

    cam->x = target_x;
    cam->y = target_y + camera_world_h(cam) * 0.5f;
}

void camera_view(const Camera *cam, mat4 out)
{
    glm_mat4_identity(out);
    glm_translate(out, (vec3){ -cam->x, -cam->y, 0.0f });
}

void camera_proj(const Camera *cam, int width, int height, mat4 out)
{
    const float hh = camera_world_h(cam) * 0.5f;
    const float hw = hh * ((float)width / (float)height);
    glm_ortho(-hw, hw, -hh, hh, -1, 1, out);
}

void camera_zoom_step(Camera *cam, float dir)
{
    const float step = (dir > 0) ? -1.0f : 1.0f;
    cam->vt_target = clampf(cam->vt_target + step, CAMERA_MIN_TILES, CAMERA_MAX_TILES);
}
