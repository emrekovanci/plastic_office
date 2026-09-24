#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

typedef struct Camera {
    int viewport_w;
    int viewport_h;

    float x;
    float y;
    float zoom;
    float zoom_target;
} Camera;

void camera_init(Camera *cam);
void camera_update(Camera *cam, float dt, float target_x);
void camera_view(const Camera *cam, mat4 out);
void camera_proj(const Camera *cam, mat4 out);

void camera_set_zoom(Camera *cam, float dir);
float camera_min_zoom(const Camera *cam);
float camera_max_zoom(const Camera *cam);

#endif
