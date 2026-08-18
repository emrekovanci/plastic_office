#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>
#include <SDL3/SDL.h>

#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080

typedef struct GameWindow GameWindow;
struct GameWindow {
    SDL_Window *handle;
    SDL_GLContext gl_context;
};

bool window_init(GameWindow *game_window);
void window_destroy(GameWindow *game_window);
void window_swap(GameWindow *game_window);
void window_get_pixel_size(GameWindow *game_window, int *w, int *h);

#endif
