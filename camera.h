#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

typedef struct Camera {
    float x, y;
    float sub_x, sub_y;
} Camera;

void camera_init(Camera *cam);
void camera_update(Camera *cam, float dt, float target_x, float target_y);
void camera_view(const Camera *cam, mat4 out);
void camera_proj(const Camera *cam, int width, int height, mat4 out);
void camera_zoom_step(Camera *cam, float dir);

#endif
