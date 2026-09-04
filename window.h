#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>
#include <SDL3/SDL.h>

typedef struct GameWindowConfig GameWindowConfig;
struct GameWindowConfig {
    int width;
    int height;
    bool fullscreen;
    bool vsync;
    bool debugging;
};

typedef struct GameWindow GameWindow;
struct GameWindow {
    SDL_Window *handle;
    SDL_GLContext gl_context;
};

bool window_init(GameWindow *game_window, const GameWindowConfig *config);
void window_destroy(GameWindow *game_window);
bool window_is_fullscreen(GameWindow *game_window);
void window_toggle_fullscreen(GameWindow *game_window);
void window_swap(GameWindow *game_window);
void window_get_pixel_size(GameWindow *game_window, int *w, int *h);

#endif
