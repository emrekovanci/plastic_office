#include <glad/glad.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cglm/cglm.h>
#include <stdarg.h>

#include "window.h"
#include "camera.h"
#include "shader.h"
#include "tilemap.h"
#include "player.h"
#include "input.h"
#include "sprite.h"
#include "constants.h"
#include "font.h"
#include "renderer.h"

// general
Shader g_shader_sprite;
Shader g_shader_default;
Camera g_camera;
Tilemap g_tilemap;
Player g_player;
Input g_input;

// text rendering
Font g_font;
Shader g_shader_text;

// backgrounds
Texture g_tex_sky;
Texture g_tex_bg1;
Texture g_tex_bg2;
Sprite g_spr_sky;
Sprite g_spr_bg1;
Sprite g_spr_bg2;

static void sync_vp(GameWindow *game_window)
{
    int w, h;
    window_get_pixel_size(game_window, &w, &h);

    Shader *shaders[] = {
        &g_shader_text,
        &g_shader_default,
        &g_shader_sprite
    };
    renderer_viewport_resize(w, h, &g_camera, shaders, 3);
}

static void debug_line(float y, const char* fmt, ...)
{
    char buf[64];
    va_list args;
    va_start(args, fmt);
    SDL_vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    font_render_text(&g_font, &g_shader_text, buf, 20.0f, y, 1.0f, WHITE);
}

static void render_debug_panel()
{
    debug_line(220.0f, "Zoom: %.4f", g_camera.zoom);
    debug_line(180.0f, "DX: %.2f", g_player.dx);
    debug_line(140.0f, "DY: %.2f", g_player.dy);
    debug_line(100.0f, "Anim Frame: %d", g_player.anim_frame);
    debug_line(60.0f, "Tile X: %.2f", g_player.cx + g_player.xr);
    debug_line(20.0f, "Tile Y: %.2f", g_player.cy + g_player.yr);
}

typedef struct AppContext AppContext;
struct AppContext {
    RenderState render_state;
    float timescale;
    Uint64 last_tick;
    double accumulator;
    GameWindow window;
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
    app->timescale = 1.0f;
    
    if (!window_init(&app->window, true)) {
        SDL_Log("Failed to create game window!");
        return SDL_APP_FAILURE;
    }

    app->last_tick = SDL_GetTicks();

    camera_init(&g_camera);

    font_load(&g_font, "assets/fonts/NotoSans-Medium.ttf", 32, 512);

    if (!tilemap_build(&g_tilemap))
        return SDL_APP_FAILURE;

    texture_load("assets/Sky.png", &g_tex_sky);
    texture_load("assets/Background_Layer1.png", &g_tex_bg1);
    texture_load("assets/Background_Layer2.png", &g_tex_bg2);

    sprite_create(&g_spr_sky);
    sprite_create(&g_spr_bg1);
    sprite_create(&g_spr_bg2);

    player_init(&g_player);

    shader_load(&g_shader_text, "assets/shaders/text_vs.glsl", "assets/shaders/text_fs.glsl");
    shader_bind(&g_shader_text);
    shader_set_int(&g_shader_text, "text", 0);

    shader_load(&g_shader_default, "assets/shaders/default_vs.glsl", "assets/shaders/default_fs.glsl");
    shader_bind(&g_shader_default);
    shader_set_int(&g_shader_default, "texture1", 0);

    shader_load(&g_shader_sprite, "assets/shaders/sprite_vs.glsl", "assets/shaders/sprite_fs.glsl");
    shader_bind(&g_shader_sprite);
    shader_set_int(&g_shader_sprite, "texture1", 0);

    sync_vp(&app->window);

    // initial zoom
    g_camera.zoom = SDL_clamp(g_camera.zoom, camera_min_zoom(&g_camera), camera_max_zoom(&g_camera));
    g_camera.zoom_target = g_camera.zoom;

    // renderer
    renderer_init(&app->render_state);

    SDL_Log("Viewport size: W: %d, H: %d", g_camera.viewport_w, g_camera.viewport_h);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    AppContext *app = appstate;

    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;

    if (event->type == SDL_EVENT_KEY_DOWN && !event->key.repeat) {
        switch (event->key.key) {
            case SDLK_F1: renderer_toggle_wireframe(&app->render_state); break;
            case SDLK_F2: tilemap_toggle_collision_visibility(&g_tilemap); break;
            case SDLK_1: app->timescale = 1.0f; break;
            case SDLK_2: app->timescale = 0.5f; break;
            case SDLK_3: app->timescale = 0.1f; break;
            case SDLK_4: app->timescale = 0.0f; break;
            case SDLK_ESCAPE: return SDL_APP_SUCCESS;
            default: break;
        }
    }

    if (event->type == SDL_EVENT_MOUSE_WHEEL)
        camera_set_zoom(&g_camera, event->wheel.y);

    if (event->type == SDL_EVENT_WINDOW_RESIZED) {
        sync_vp(&app->window);
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
    dt *= app->timescale;
    app->last_tick = current_tick;
    app->accumulator += dt;

    input_update(&g_input);

    while (app->accumulator >= FIXED_TIMESTEP) {
        player_fixed_update(&g_player, FIXED_TIMESTEP, &g_input, &g_tilemap);
        app->accumulator -= FIXED_TIMESTEP;
    }

    const float alpha = (float)(app->accumulator / FIXED_TIMESTEP);
    player_update(&g_player, dt, &g_input, alpha, &g_tilemap);
    camera_update(&g_camera, dt, g_player.render_xx);

    // clear
    renderer_clear(0.0f, 0.0f, 0.0f, 1.0f);

    // render background
    shader_bind(&g_shader_default);
    renderer_draw_parallax_layer(&g_shader_default, &g_camera, 0.0f, &g_spr_sky, &g_tex_sky);
    renderer_draw_parallax_layer(&g_shader_default, &g_camera, 0.01f, &g_spr_bg1, &g_tex_bg1);
    renderer_draw_parallax_layer(&g_shader_default, &g_camera, 0.2f, &g_spr_bg2, &g_tex_bg2);

    // render tilemap
    mat4 view;
    camera_view(&g_camera, view);
    shader_set_mat4(&g_shader_default, "view", (float*)view);
    tilemap_render(&g_tilemap, &g_shader_default);

    // render player
    shader_bind(&g_shader_sprite);
    shader_set_mat4(&g_shader_sprite, "view", (float*)view);
    player_render(&g_player, &g_shader_sprite);

    // debugging
    render_debug_panel();

    window_swap(&app->window);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    AppContext *app = appstate;

    texture_destroy(&g_tex_bg2);
    texture_destroy(&g_tex_bg1);
    texture_destroy(&g_tex_sky);
    sprite_destroy(&g_spr_bg2);
    sprite_destroy(&g_spr_bg1);
    sprite_destroy(&g_spr_sky);

    tilemap_destroy(&g_tilemap);
    player_destroy(&g_player);
    shader_destroy(&g_shader_default);
    shader_destroy(&g_shader_sprite);

    font_destroy(&g_font);
    shader_destroy(&g_shader_text);

    window_destroy(&app->window);
    SDL_free(app);
}
