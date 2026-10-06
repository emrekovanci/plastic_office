#include "camera.h"
#include "constants.h"
#include "utils.h"

void camera_init(Camera *cam)
{        
    *cam = (Camera) { 0 };
}

void camera_update(Camera *cam, float dt, float target_x, float target_y)
{
    cam->x = roundf(target_x - VIRT_W * 0.5f);
    cam->y = target_y;
}

void camera_view(const Camera *cam, mat4 out)
{
    glm_mat4_identity(out);
    glm_translate(out, (vec3) { -cam->x, -cam->y, 0.0f });
}

void camera_proj(const Camera *cam, int width, int height, mat4 out)
{
    glm_ortho(0.0f, width, 0.0f, height, -1, 1, out);
}
