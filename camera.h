#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

typedef struct Camera Camera;
struct Camera {
    int viewport_x;
    int viewport_y;
    int viewport_w;
    int viewport_h;

    float x;
    float y;
    float zoom;
    float zoom_target;
};

void camera_init(Camera *camera);
void camera_update(Camera *camera, float dt, float target_x);
void camera_view(Camera *camera, mat4 out);
void camera_proj(Camera *camera, mat4 out);

#endif
