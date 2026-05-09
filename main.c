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
#include "constants.h"
#include "font.h"

#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080

static void set_parallax_view(Shader *shader, Camera *camera, float factor)
{
    mat4 view;
    glm_mat4_identity(view);

    vec3 translation = { -camera->x * factor, 0.0f, 0.0f };
    glm_translate(view, translation);
    shader_set_mat4(shader, "view", (float*)view);
}

static void render_background(Sprite *sprite, Texture *texture, Shader *shader, Camera *camera, float factor)
{
    set_parallax_view(shader, camera, factor);
    sprite_render(sprite,
                  texture,
                  shader,
                  0.0f, 0.0f,
                  (float)texture->width * TILE_SCALE, (float)texture->height * TILE_SCALE,
                  0.0f,
                  0.0f, 0.0f);
}

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
Texture g_bg_sky_tex;
Sprite g_bg_sky_spr;
Texture g_bg2_tex;
Sprite g_bg2_spr;
Texture g_bg1_tex;
Sprite g_bg1_spr;

static void sync_vp(SDL_Window *window, Camera *camera)
{
    SDL_GetWindowSizeInPixels(window, &camera->viewport_w, &camera->viewport_h);

    glViewport(camera->viewport_x, camera->viewport_y, camera->viewport_w, camera->viewport_h);

    mat4 proj;
    camera_proj(camera, proj);

    shader_bind(&g_shader_text);
    shader_set_mat4(&g_shader_text, "projection", (float*)proj);
    
    shader_bind(&g_shader_default);
    shader_set_mat4(&g_shader_default, "projection", (float*)proj);

    shader_bind(&g_shader_sprite);
    shader_set_mat4(&g_shader_sprite, "projection", (float*)proj);

}

typedef struct AppContext AppContext;
struct AppContext {
    bool wireframe;
    float timescale;
    Uint64 last_tick;
    double accumulator;
    SDL_Window *window;
    SDL_GLContext gl_context;
};

static void toggle_wireframe(AppContext *app)
{
    app->wireframe = !app->wireframe;
    if (app->wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

static void toggle_collision_visibility()
{
    TilemapLayer *collision_layer = &g_tilemap.layers[MAP_LAYERS - 1];
    collision_layer->visible = !collision_layer->visible;
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
    app->timescale = 1.0f;

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

    camera_init(&g_camera);

    font_load(&g_font, "assets/fonts/NotoSans-Medium.ttf", 32, 512);

    if (!tilemap_build(&g_tilemap))
        return SDL_APP_FAILURE;

    texture_load("assets/Sky.png", &g_bg_sky_tex);
    texture_load("assets/Background_Layer1.png", &g_bg1_tex);
    texture_load("assets/Background_Layer2.png", &g_bg2_tex);

    sprite_init(&g_bg_sky_spr);
    sprite_init(&g_bg2_spr);
    sprite_init(&g_bg1_spr);

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

    sync_vp(app->window, &g_camera);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    SDL_Log("Viewport size: W: %d, H: %d", g_camera.viewport_w, g_camera.viewport_h);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    AppContext *app = appstate;

    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;

    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
            case SDLK_F1: toggle_wireframe(app); break;
            case SDLK_F2: toggle_collision_visibility(); break;
            case SDLK_1: app->timescale = 1.0f; break;
            case SDLK_2: app->timescale = 0.5f; break;
            case SDLK_3: app->timescale = 0.1f; break;
            case SDLK_4: app->timescale = 0.0f; break;
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
        sync_vp(app->window, &g_camera);
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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // render background
    shader_bind(&g_shader_default);
    render_background(&g_bg_sky_spr, &g_bg_sky_tex, &g_shader_default, &g_camera, 0.0f);
    render_background(&g_bg1_spr, &g_bg1_tex, &g_shader_default, &g_camera, 0.01f);
    render_background(&g_bg2_spr, &g_bg2_tex, &g_shader_default, &g_camera, 0.2f);

    // render foreground
    mat4 view;
    camera_view(&g_camera, view);
    shader_set_mat4(&g_shader_default, "view", (float*)view);
    tilemap_render(&g_tilemap, &g_shader_default);

    shader_bind(&g_shader_sprite);
    shader_set_mat4(&g_shader_sprite, "view", (float*)view);
    player_render(&g_player, &g_shader_sprite);

    // debug panel
    char debug_text[64];
    SDL_snprintf(debug_text, sizeof(debug_text), "DX: %.2f", g_player.dx);
    font_render_text(&g_font, &g_shader_text, debug_text, 20.0f, 180.0f, 1.0f, WHITE);
    SDL_snprintf(debug_text, sizeof(debug_text), "DY: %.2f", g_player.dy);
    font_render_text(&g_font, &g_shader_text, debug_text, 20.0f, 140.0f, 1.0f, WHITE);
    SDL_snprintf(debug_text, sizeof(debug_text), "Anim Frame: %d", g_player.anim_frame);
    font_render_text(&g_font, &g_shader_text, debug_text, 20.0f, 100.0f, 1.0f, WHITE);
    SDL_snprintf(debug_text, sizeof(debug_text), "Tile X: %.2f", g_player.cx + g_player.xr);
    font_render_text(&g_font, &g_shader_text, debug_text, 20.0f, 60.0f, 1.0f, WHITE);
    SDL_snprintf(debug_text, sizeof(debug_text), "Tile Y: %.2f", g_player.cy + g_player.yr);
    font_render_text(&g_font, &g_shader_text, debug_text, 20.0f, 20.0f, 1.0f, WHITE);

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

    tilemap_destroy(&g_tilemap);
    player_destroy(&g_player);
    shader_destroy(&g_shader_default);
    shader_destroy(&g_shader_sprite);

    font_destroy(&g_font);
    shader_destroy(&g_shader_text);

    SDL_GL_DestroyContext(app->gl_context);
    SDL_DestroyWindow(app->window);
    SDL_free(app);
}
