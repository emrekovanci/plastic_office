#include <glad/glad.h>
#include "window.h"

bool window_init(GameWindow *game_window)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window *window = SDL_CreateWindow("SSS", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
    if (!window) {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        return false;
    }
    game_window->handle = window;

    SDL_GLContext gl_context = SDL_GL_CreateContext(game_window->handle);
    if (!gl_context) {
        SDL_Log("Failed to create GL context: %s", SDL_GetError());
        return false;
    }
    game_window->gl_context = gl_context;

    if (!SDL_GL_MakeCurrent(game_window->handle, game_window->gl_context)) {
        SDL_Log("Failed to set up GL context into window: %s", SDL_GetError());
        return false;
    }

    if (!SDL_GL_SetSwapInterval(1))
        SDL_Log("Failed to activate VSync: %s", SDL_GetError());

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        SDL_Log("Failed to load GLAD: %s", SDL_GetError());
        return false;
    }

    return true;
}

void window_destroy(GameWindow *game_window)
{
    SDL_GL_DestroyContext(game_window->gl_context);
    SDL_DestroyWindow(game_window->handle);
}

void window_swap(GameWindow *game_window)
{
    SDL_GL_SwapWindow(game_window->handle);
}

void window_get_pixel_size(GameWindow *game_window, int *w, int *h)
{
    SDL_GetWindowSizeInPixels(game_window->handle, w, h);
}
