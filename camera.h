#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>
#include <SDL3/SDL.h>

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

void camera_reset(Camera *camera, SDL_Window *window);
void camera_follow(Camera *camera, float target_x, float target_y);
void camera_view(Camera *camera, mat4 out);
void camera_proj(Camera *camera, mat4 out);

#endif
