#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

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
#include "parallax.h"
#include "timer.h"
#include "render_target.h"

// general
Camera g_camera;
Tilemap g_tilemap;
Player g_player;

// backgrounds
Texture g_tex_bg1;
Texture g_tex_bg2;
Texture g_tex_bg3;
Texture g_tex_bg4;
Sprite g_spr_bg;

typedef struct AppContext {
    Timer timer;
    Font font;
    float ui_scale;
    Shader shader_sprite;
    Shader shader_default;
    Shader shader_text;
    Input input;
    Renderer renderer;
    RenderTarget rt;
    bool debug_overlay;
    Window window;
} AppContext;

static void debug_line(AppContext *app, float y, const char* fmt, ...)
{
    char buf[64];

    va_list args;
    va_start(args, fmt);
    SDL_vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    font_render_text(&app->font, &app->shader_text, buf,
                     20.0f * app->ui_scale,
                     y * app->ui_scale,
                     1.0f * app->ui_scale, WHITE);
}

static void render_debug_panel(AppContext *app)
{
    int vsync;
    window_get_vsync(&app->window, &vsync);

    debug_line(app, 220.0f, "VSync: %d", vsync);
    debug_line(app, 180.0f, "Anim Frame: %d", g_player.anim_frame);
    debug_line(app, 140.0f, "Grounded: %d", g_player.grounded);
    debug_line(app, 100.0f, "DX: %.2f, DY: %.2f", g_player.dx, g_player.dy);
    debug_line(app, 60.0f, "CX: %d, XR: %.2f", g_player.cx, g_player.xr);
    debug_line(app, 20.0f, "CY: %d, YR: %.2f", g_player.cy, g_player.yr);
}

SDL_AppResult app_create(AppContext *app)
{
    app->ui_scale = 1.0f;
    app->debug_overlay = true;

    WindowConfig window_config = {
        .width = WINDOW_WIDTH,
        .height = WINDOW_HEIGHT,
        .fullscreen = true,
        .vsync = true,
        .debugging = true
    };

    if (!window_init(&app->window, &window_config)) {
        SDL_Log("Failed to create window!");
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

void app_destroy(AppContext *app)
{
    window_destroy(&app->window);
}

// loads neccesary assets
SDL_AppResult app_load_assets(AppContext *app)
{
    font_load(&app->font, "assets/fonts/NotoSans-Medium.ttf", 32, 512);

    shader_load(&app->shader_text, "assets/shaders/text_vs.glsl", "assets/shaders/text_fs.glsl");
    shader_load(&app->shader_default, "assets/shaders/default_vs.glsl", "assets/shaders/default_fs.glsl");
    shader_load(&app->shader_sprite, "assets/shaders/sprite_vs.glsl", "assets/shaders/sprite_fs.glsl");

    return SDL_APP_CONTINUE;
}

// unload necceary assets
void app_unload_assets(AppContext *app)
{
    shader_destroy(&app->shader_default);
    shader_destroy(&app->shader_sprite);

    font_destroy(&app->font);
    shader_destroy(&app->shader_text);
}

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

    if (app_create(app) != SDL_APP_CONTINUE)
        return SDL_APP_FAILURE;

    app_load_assets(app);
    texture_load(&g_tex_bg1, "assets/BG/bg1.png");
    texture_load(&g_tex_bg2, "assets/BG/bg2.png");
    texture_load(&g_tex_bg3, "assets/BG/bg3.png");
    texture_load(&g_tex_bg4, "assets/BG/bg4.png");
    sprite_create(&g_spr_bg);

    renderer_init(&app->renderer);

    int w, h;
    window_get_pixel_size(&app->window, &w, &h);
    
    if (!rt_create(&app->rt, w, h, VIRT_W, VIRT_H)) {
        SDL_Log("Failed to create render target!");
        return SDL_APP_FAILURE;
    }

    camera_init(&g_camera);
    tilemap_init(&g_tilemap);
    player_init(&g_player);

    timer_init(&app->timer, FIXED_TIMESTEP);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    AppContext *app = appstate;

    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;

    if (event->type == SDL_EVENT_KEY_DOWN && !event->key.repeat) {
        switch (event->key.key) {
            case SDLK_F1: renderer_toggle_wireframe(&app->renderer); break;
            case SDLK_F2: tilemap_toggle_collision_visibility(&g_tilemap); break;
            case SDLK_F3: g_player.render_hitbox = !g_player.render_hitbox; break;
            case SDLK_F4: app->debug_overlay = !app->debug_overlay; break;
            case SDLK_1: app->timer.timescale = 1.0f; break;
            case SDLK_2: app->timer.timescale = 0.5f; break;
            case SDLK_3: app->timer.timescale = 0.1f; break;
            case SDLK_4: app->timer.timescale = 0.0f; break;
            case SDLK_RETURN:
                if (event->key.mod & SDL_KMOD_ALT)
                    window_toggle_fullscreen(&app->window);
                break;
            case SDLK_ESCAPE: return SDL_APP_SUCCESS;
            default: break;
        }
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppContext *app = appstate;

    // ---------
    // game loop
    // ---------
    float dt = timer_tick(&app->timer);
    input_update(&app->input);

    while (timer_step(&app->timer))
        player_fixed_update(&g_player, FIXED_TIMESTEP, &app->input, &g_tilemap);

    const float alpha = timer_alpha(&app->timer);
    player_update(&g_player, dt, &app->input, alpha, &g_tilemap);
    camera_update(&g_camera, dt, g_player.render_xx, 0.0f);

    int w, h;
    window_get_pixel_size(&app->window, &w, &h);
    app->ui_scale = (float)h / UI_REFERENCE_HEIGHT;

    // parallax_render(&app->shader_default, VIRT_W, VIRT_H, 0.0f,               &g_spr_bg, &g_tex_bg1);
    // parallax_render(&app->shader_default, VIRT_W, VIRT_H, g_camera.x * 0.05f, &g_spr_bg, &g_tex_bg2);
    // parallax_render(&app->shader_default, VIRT_W, VIRT_H, g_camera.x * 0.3f,  &g_spr_bg, &g_tex_bg3);
    // parallax_render(&app->shader_default, VIRT_W, VIRT_H, g_camera.x * 0.3f,  &g_spr_bg, &g_tex_bg4);
    
    rt_begin(&app->rt);

    renderer_use_world(&app->renderer, &g_camera, app->rt.w, app->rt.h);
    shader_bind(&app->shader_default);
    tilemap_render(&g_tilemap, &app->shader_default);

    shader_bind(&app->shader_sprite);
    player_render(&g_player, &app->shader_sprite);

    rt_present(&app->rt, w, h, &g_camera);

    if (app->debug_overlay) {
        renderer_use_screen(&app->renderer, w, h);
        render_debug_panel(app);
    }

    window_swap(&app->window);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    AppContext *app = appstate;

    sprite_destroy(&g_spr_bg);
    texture_destroy(&g_tex_bg4);
    texture_destroy(&g_tex_bg3);
    texture_destroy(&g_tex_bg2);
    texture_destroy(&g_tex_bg1);

    tilemap_destroy(&g_tilemap);
    player_destroy(&g_player);

    rt_destroy(&app->rt);
    renderer_destroy(&app->renderer);
    app_unload_assets(app);
    app_destroy(app);

    SDL_free(app);
    SDL_Quit();
}
