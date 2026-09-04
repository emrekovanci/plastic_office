#include <glad/glad.h>
#include "window.h"
#include "constants.h"

static void GLAPIENTRY gl_debug_callback(
    GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar *message, const void *user_param)
{
    (void)length;
    (void)user_param;

    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    const char *source_str;
    switch (source) {
        case GL_DEBUG_SOURCE_API:             source_str = "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source_str = "Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: source_str = "Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     source_str = "Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     source_str = "Application"; break;
        default:                              source_str = "Other"; break;
    }

    const char *type_str;
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:               type_str = "Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_str = "Deprecated Behavior"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_str = "Undefined Behavior"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         type_str = "Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         type_str = "Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              type_str = "Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          type_str = "Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           type_str = "Pop Group"; break;
        default:                                type_str = "Other"; break;
    }

    const char *severity_str;
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:         severity_str = "HIGH"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       severity_str = "MEDIUM"; break;
        case GL_DEBUG_SEVERITY_LOW:          severity_str = "LOW"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: severity_str = "NOTIFICATION"; break;
        default:                             severity_str = "UNKNOWN"; break;
    }

    SDL_Log("[GL %s] source=%s type=%s id=%u: %s", severity_str, source_str, type_str, id, message);
}

bool window_init(GameWindow *game_window, bool debugging)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // debug output is core since OpenGL version 4.3
    // which means you'll find this functionality on any machine that runs OpenGL 4.3 or higher.
    if (debugging)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

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

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_debug_callback, NULL);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
        SDL_Log("OpenGL Debug Context enabled");
    } else {
        SDL_Log("OpenGL Debug Context disabled");
    }

    SDL_Log("OpenGL Version: %s", glGetString(GL_VERSION));
    SDL_Log("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    SDL_Log("Renderer: %s", glGetString(GL_RENDERER));
    SDL_Log("Vendor: %s", glGetString(GL_VENDOR));

    return true;
}

void window_destroy(GameWindow *game_window)
{
    SDL_GL_DestroyContext(game_window->gl_context);
    SDL_DestroyWindow(game_window->handle);
}

bool window_is_fullscreen(GameWindow *game_window)
{
    SDL_WindowFlags flags = SDL_GetWindowFlags(game_window->handle);
    return (flags & SDL_WINDOW_FULLSCREEN) != 0;
}

void window_toggle_fullscreen(GameWindow *game_window)
{
    bool is_fullscreen = window_is_fullscreen(game_window);

    if (!SDL_SetWindowFullscreen(game_window->handle, !is_fullscreen)) {
        SDL_Log("Failed to toggle fullscreen: %s", SDL_GetError());
        return;
    }

    SDL_Log("Fullscreen: %s", !is_fullscreen ? "ON": "OFF");
}

void window_swap(GameWindow *game_window)
{
    SDL_GL_SwapWindow(game_window->handle);
}

void window_get_pixel_size(GameWindow *game_window, int *w, int *h)
{
    SDL_GetWindowSizeInPixels(game_window->handle, w, h);
}
