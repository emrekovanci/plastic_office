#include "camera.h"
#include "constants.h"
#include "utils.h"

void camera_init(Camera *cam)
{        
    *cam = (Camera) { 0 };
}

void camera_update(Camera *cam, float dt, float target_x, float target_y)
{
    const float px = (float)TILE_SCALE;
    const float ty = target_y + VIRT_H * 0.5f;

    cam->x = roundf(target_x / px) * px;
    cam->y = roundf(ty / px) * px;

    cam->sub_x = (target_x - cam->x) / px;
    cam->sub_y = (ty - cam->y) / px;
}

void camera_view(const Camera *cam, mat4 out)
{
    glm_mat4_identity(out);

    glm_translate(out, (vec3) { -cam->x, -cam->y, 0.0f });
}

void camera_proj(const Camera *cam, int width, int height, mat4 out)
{
    const float hw = VIRT_W * 0.5f;
    const float hh = VIRT_H * 0.5f;
    glm_ortho(-hw, hw, -hh, hh, -1, 1, out);
}
