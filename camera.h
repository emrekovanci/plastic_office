#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

#define CAMERA_ZOOM_STEP         1.1f
#define CAMERA_MIN_VISIBLE_TILES 6.0f

typedef struct Camera Camera;
struct Camera {
    int viewport_w;
    int viewport_h;

    float x;
    float y;
    float zoom;
    float zoom_target;
};

void camera_init(Camera *c);
void camera_update(Camera *c, float dt, float target_x);
void camera_view(Camera *c, mat4 out);
void camera_proj(Camera *c, mat4 out);

void camera_set_zoom(Camera *c, float dir);
float camera_min_zoom(const Camera *c, int map_rows);
float camera_max_zoom(const Camera *c);

#endif
