#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>
#include <SDL3/SDL_video.h>

typedef struct WindowConfig {
    int width;
    int height;
    bool fullscreen;
    bool vsync;
    bool debugging;
} WindowConfig;

typedef struct Window {
    SDL_Window *handle;
    SDL_GLContext gl_context;
} Window;

bool window_init(Window *win, const WindowConfig *config);
void window_destroy(Window *win);
bool window_is_fullscreen(Window *win);
void window_toggle_fullscreen(Window *win);
void window_swap(Window *win);

void window_get_pixel_size(Window *win, int *w, int *h);
void window_get_logical_size(Window *win, int *w, int *h);
void window_get_vsync(Window *win, int *vsync);

#endif
