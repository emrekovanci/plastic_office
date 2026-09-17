#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>
#include <SDL3/SDL.h>

typedef struct WindowConfig WindowConfig;
struct WindowConfig {
    int width;
    int height;
    bool fullscreen;
    bool vsync;
    bool debugging;
};

typedef struct Window Window;
struct Window {
    SDL_Window *handle;
    SDL_GLContext gl_context;
};

bool window_init(Window *win, const WindowConfig *config);
void window_destroy(Window *win);
bool window_is_fullscreen(Window *win);
void window_toggle_fullscreen(Window *win);
void window_swap(Window *win);

void window_get_pixel_size(Window *win, int *w, int *h);
void window_get_logical_size(Window *win, int *w, int *h);

#endif
