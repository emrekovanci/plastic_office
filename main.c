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
#include "utils.h"

#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080

static void sync_vp(Shader *shader, Camera *camera)
{
    glViewport(camera->viewport_x, camera->viewport_y, camera->viewport_w, camera->viewport_h);

    mat4 proj;
    camera_proj(camera, proj);
    shader_set_mat4(shader, "projection", (float*)proj);
}

static void set_parallax_view(Shader *shader, Camera *camera, float factor)
{
    mat4 view;
    glm_mat4_identity(view);

    vec3 translation = { -camera->x * factor, 0.0f, 0.0f };
    glm_translate(view, translation);
    shader_set_mat4(shader, "view", (float*)view);
}

Shader g_shader;
Camera g_camera;
Tilemap g_map;
Player g_player;
Input g_input;

Texture g_bg_sky_tex;
Sprite g_bg_sky_spr;

Texture g_bg2_tex;
Sprite g_bg2_spr;

Texture g_bg1_tex;
Sprite g_bg1_spr;

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

    SDL_Window *window = SDL_CreateWindow("SSS", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
    if (!window) {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    app->window = window;

    SDL_GLContext gl_context = SDL_GL_CreateContext(app->window);
    if (!gl_context) {
        SDL_Log("Failed to create gl context: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    app->gl_context = gl_context;

    if (!SDL_GL_MakeCurrent(app->window, app->gl_context)) {
        SDL_Log("Failed to set up OpenGL context into window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_GL_SetSwapInterval(1))
        SDL_Log("Failed to activate VSync: %s", SDL_GetError());
    
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

    shader_load(&g_shader, "assets/vert.glsl", "assets/frag.glsl");
    texture_load("assets/Sky.png", &g_bg_sky_tex);
    texture_load("assets/Background_Layer1.png", &g_bg1_tex);
    texture_load("assets/Background_Layer2.png", &g_bg2_tex);

    sprite_init(&g_bg_sky_spr);
    sprite_init(&g_bg2_spr);
    sprite_init(&g_bg1_spr);

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
        g_camera.zoom_target *= (event->wheel.y > 0) ? 1.1f : 0.9f;
        float min_zoom = (float)g_camera.viewport_h / (MAP_ROWS * TILE_RENDER_SIZE);
        min_zoom = roundf(min_zoom * 1000.0f) / 1000.0f;
        g_camera.zoom_target = SDL_clamp(g_camera.zoom_target, min_zoom, 2.5f);
        SDL_Log("Min zoom: %f, Camera zoom: %f", min_zoom, g_camera.zoom);
    }

    if (event->type == SDL_EVENT_WINDOW_RESIZED) {
        sync_vp(&g_shader, &g_camera);
        SDL_Log("Window resized: W: %d, H: %d", event->window.data1, event->window.data2);
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppContext *app = appstate;

    const Uint64 current_tick = SDL_GetTicks();
    float dt = (float)(current_tick - app->last_tick) / SDL_MS_PER_SECOND;
    dt = SDL_min(dt, 0.25f);
    app->last_tick = current_tick;
    app->accumulator += dt;

    input_update(&g_input);

    while (app->accumulator >= FIXED_TIMESTEP) {
        player_fixed_update(&g_player, FIXED_TIMESTEP, &g_input, &g_camera);
        app->accumulator -= FIXED_TIMESTEP;
    }

    const float alpha = (float)(app->accumulator / FIXED_TIMESTEP);
    player_update(&g_player, dt, &g_input, &g_camera, alpha);

    // camera zoom & movement
    float zoom = lerpf(g_camera.zoom, g_camera.zoom_target, 1.0f - expf(-10.0f * dt));
    zoom = roundf(zoom * 1000.0f) / 1000.0f;
    g_camera.zoom = zoom;
    const float target_y = (g_camera.viewport_h * 0.5f) / g_camera.zoom;
    camera_follow(&g_camera, g_player.render_xx + PLAYER_RENDER_SIZE * 0.5f, target_y);

    // clear
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // render background
    set_parallax_view(&g_shader, &g_camera, 0.0f);
    sprite_render(&g_bg_sky_spr,
                  &g_bg_sky_tex,
                  &g_shader,
                  0.0f, 0.0f,
                  (float)g_bg_sky_tex.width * TILE_SCALE, (float)g_bg_sky_tex.height * TILE_SCALE,
                  0.0f);

    set_parallax_view(&g_shader, &g_camera, 0.01f);
    sprite_render(&g_bg1_spr,
                  &g_bg1_tex,
                  &g_shader,
                  0.0f, 0.0f,
                  (float)g_bg1_tex.width * TILE_SCALE, (float)g_bg1_tex.height * TILE_SCALE,
                  0.0f);

    set_parallax_view(&g_shader, &g_camera, 0.2f);
    sprite_render(&g_bg2_spr,
                  &g_bg2_tex,
                  &g_shader,
                  0.0f, 0.0f,
                  (float)g_bg2_tex.width * TILE_SCALE, (float)g_bg2_tex.height * TILE_SCALE,
                  0.0f);

    // render foreground
    mat4 view;
    camera_view(&g_camera, view);
    shader_set_mat4(&g_shader, "view", (float*)view);
    tilemap_render(&g_map, &g_shader);
    player_render(&g_player, &g_shader);

    SDL_GL_SwapWindow(app->window);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    AppContext *app = appstate;

    sprite_destroy(&g_bg1_spr);
    texture_destroy(&g_bg1_tex);
    sprite_destroy(&g_bg2_spr);
    texture_destroy(&g_bg2_tex);
    sprite_destroy(&g_bg_sky_spr);
    texture_destroy(&g_bg_sky_tex);

    tilemap_destroy(&g_map);
    player_destroy(&g_player);
    shader_destroy(&g_shader);

    SDL_GL_DestroyContext(app->gl_context);
    SDL_DestroyWindow(app->window);
    SDL_free(app);
}
