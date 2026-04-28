#include <glad/glad.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cglm/cglm.h>

#include "camera.h"
#include "shader.h"
#include "tilemap.h"
#include "player.h"
#include "input.h"
#include "sprite.h"

#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080

static void sync_vp(Shader *shader, Camera *camera)
{
    glViewport(camera->viewport_x, camera->viewport_y, camera->viewport_w, camera->viewport_h);

    mat4 proj;
    camera_proj(camera, proj);
    shader_set_mat4(shader, "projection", (float*)proj);
}

Shader g_shader;
Camera g_camera;
Tilemap g_map;
Player g_player;
Input g_input;

Texture g_bg_0_tex;
Sprite g_bg_0;

Texture g_bg_1_tex;
Sprite g_bg_1;

Texture g_bg_2_tex;
Sprite g_bg_2;

Texture g_bg_sky_tex;
Sprite g_bg_sky;

typedef struct AppContext AppContext;
struct AppContext {
    Uint64 last_tick;
    double accumulator;
    SDL_Window *window;
    SDL_GLContext gl_context;
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("SSS", "1.0", "com.kovanci.SSS");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    AppContext *app = SDL_calloc(1, sizeof(*app));
    if (!app) {
        SDL_Log("Failed to allocate memory for App: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    *appstate = app;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    if (!(app->window = SDL_CreateWindow("SSS", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL))) {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!(app->gl_context = SDL_GL_CreateContext(app->window))) {
        SDL_Log("Failed to create gl context: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_GL_MakeCurrent(app->window, app->gl_context)) {
        SDL_Log("Failed to set up OpenGL context into window.");
        return SDL_APP_FAILURE;
    }

    SDL_GL_SetSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        SDL_Log("Failed to load glad: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    app->last_tick = SDL_GetTicks();

    SDL_Log("OpenGL Version: %s", glGetString(GL_VERSION));
    SDL_Log("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    SDL_Log("Renderer: %s", glGetString(GL_RENDERER));
    SDL_Log("Vendor: %s", glGetString(GL_VENDOR));

    camera_reset(&g_camera, app->window);
    SDL_Log("Viewport Size in pixels: %d, %d", g_camera.viewport_w, g_camera.viewport_h);

    if (!tilemap_build(&g_map))
        return SDL_APP_FAILURE;

    if (!shader_load(&g_shader, "assets/vert.glsl", "assets/frag.glsl"))
        return SDL_APP_FAILURE;

    if (!texture_load("assets/Background.png", &g_bg_0_tex))
        return SDL_APP_FAILURE;

    if (!texture_load("assets/Background_Layer1.png", &g_bg_1_tex))
        return SDL_APP_FAILURE;

    if (!texture_load("assets/Background_Layer2.png", &g_bg_2_tex))
        return SDL_APP_FAILURE;

    if (!texture_load("assets/Sky.png", &g_bg_sky_tex))
        return SDL_APP_FAILURE;

    sprite_init(&g_bg_0);
    sprite_init(&g_bg_1);
    sprite_init(&g_bg_2);
    sprite_init(&g_bg_sky);

    shader_bind(&g_shader);
    shader_set_int(&g_shader, "texture1", 0);

    player_init(&g_player);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    sync_vp(&g_shader, &g_camera);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    AppContext *app = appstate;

    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;

    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
        case SDLK_F1: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
        case SDLK_F2: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
        case SDLK_ESCAPE: return SDL_APP_SUCCESS;
        default: break;
        }
    }

    if (event->type == SDL_EVENT_MOUSE_WHEEL) {
        g_camera.zoom *= (event->wheel.y > 0) ? 1.1f : 0.9f;
        g_camera.zoom = SDL_clamp(g_camera.zoom, 0.25f, 4.0f);
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppContext *app = appstate;

    const Uint64 current_tick = SDL_GetTicks();
    float delta_time = (float)(current_tick - app->last_tick) / SDL_MS_PER_SECOND;
    delta_time = SDL_min(delta_time, 0.25f);
    app->last_tick = current_tick;
    app->accumulator += delta_time;

    input_update(&g_input);

    while (app->accumulator >= FIXED_TIMESTEP)
    {
        player_fixed_update(&g_player, FIXED_TIMESTEP, &g_input, &g_camera);
        app->accumulator -= FIXED_TIMESTEP;
    }

    const float alpha = (float)(app->accumulator / FIXED_TIMESTEP);
    player_update(&g_player, delta_time, &g_input, &g_camera, alpha);
    camera_follow(&g_camera, g_player.render_xx, g_camera.viewport_h * 0.5f);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    mat4 view;
    camera_view(&g_camera, view);
    shader_set_mat4(&g_shader, "view", (float*)view);

    // render background
    sprite_render(&g_bg_0,
                  &g_bg_0_tex,
                  &g_shader,
                  0.0f, 0.0f,
                  (float)g_bg_0_tex.width * TILE_SCALE, (float)g_bg_0_tex.height * TILE_SCALE,
                  0.0f);

    sprite_render(&g_bg_1,
                  &g_bg_1_tex,
                  &g_shader,
                  0.0f, 0.0f,
                  (float)g_bg_1_tex.width * TILE_SCALE, (float)g_bg_1_tex.height * TILE_SCALE,
                  0.0f);

    sprite_render(&g_bg_2,
                  &g_bg_2_tex,
                  &g_shader,
                  0.0f, 0.0f,
                  (float)g_bg_2_tex.width * TILE_SCALE, (float)g_bg_2_tex.height * TILE_SCALE,
                  0.0f);

    sprite_render(&g_bg_sky,
                  &g_bg_sky_tex,
                  &g_shader,
                  0.0f, g_bg_0_tex.height * TILE_SCALE * 0.5f,
                  (float)g_bg_sky_tex.width * TILE_SCALE, (float)g_bg_sky_tex.height * TILE_SCALE,
                  0.0f);

    // render foreground
    tilemap_render(&g_map, &g_shader);
    player_render(&g_player, &g_shader);

    SDL_GL_SwapWindow(app->window);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    AppContext *app = appstate;

    shader_destroy(&g_shader);
    tilemap_destroy(&g_map);
    player_destroy(&g_player);

    sprite_destroy(&g_bg_0);
    texture_destroy(&g_bg_0_tex);
    sprite_destroy(&g_bg_1);
    texture_destroy(&g_bg_1_tex);
    sprite_destroy(&g_bg_2);
    texture_destroy(&g_bg_2_tex);
    sprite_destroy(&g_bg_sky);
    texture_destroy(&g_bg_sky_tex);

    SDL_GL_DestroyContext(app->gl_context);
    SDL_DestroyWindow(app->window);
    SDL_free(app);
}
