#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

#define CAMERA_ZOOM_STEP 1.1f

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

float camera_min_zoom(const Camera *camera, int map_rows);
float camera_max_zoom(const Camera *camera);

#endif
