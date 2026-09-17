#include <glad/glad.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cglm/cglm.h>

#include "camera.h"
#include "shader.h"
#include "font.h"

#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080

Camera g_camera;
Font g_font_sc;
Font g_font;
Shader g_shader_text;

static void sync_vp(SDL_Window *window, Camera *camera)
{
    SDL_GetWindowSizeInPixels(window, &camera->viewport_w, &camera->viewport_h);

    glViewport(0, 0, camera->viewport_w, camera->viewport_h);

    mat4 proj;
    camera_proj(camera, proj);

    shader_bind(&g_shader_text);
    shader_set_mat4(&g_shader_text, "projection", (float*)proj);
}

typedef struct AppContext AppContext;
struct AppContext {
    Uint64 last_tick;
    double accumulator;
    SDL_Window *window;
    SDL_GLContext gl_context;
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("SSS_Renderer_Font_Tests", "1.0", "com.kovanci.SSS_Renderer_Font_Tests");

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

    camera_init(&g_camera);

    font_load(&g_font, "assets/fonts/NotoSans-Medium.ttf", 24, 2048);
    font_load(&g_font_sc, "assets/fonts/NotoSansSC-Medium.ttf", 24, 2048);

    shader_load(&g_shader_text, "assets/shaders/text_vs.glsl", "assets/shaders/text_fs.glsl");
    shader_bind(&g_shader_text);
    shader_set_int(&g_shader_text, "text", 0);

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

    if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_ESCAPE)
        return SDL_APP_SUCCESS;

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
    app->last_tick = current_tick;
    app->accumulator += dt;

    camera_update(&g_camera, dt, 50.0f);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    const float sh = g_camera.viewport_h - 40.0f;

    // Türkçe
    const char *turkish_upper = "A, B, C, Ç, D, E, F, G, Ğ, H, I, İ, J, K, L, M, N, O, Ö, P, R, S, Ş, T, U, Ü, V, Y, Z";
    const char *turkish_lower = "a, b, c, ç, d, e, f, g, ğ, h, ı, i, j, k, l, m, n, o, ö, p, r, s, ş, t, u, ü, v, y, z";
    font_render_text(&g_font, &g_shader_text, "Türkçe", 20.0f, sh, 1.0f, WHITE);
    font_render_text(&g_font, &g_shader_text, turkish_upper, 20.0f, sh - 40.0f, 1.0f, WHITE);
    font_render_text(&g_font, &g_shader_text, turkish_lower, 20.0f, sh - 80.0f, 1.0f, WHITE);

    // English
    const char *english_upper = "A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z";
    const char *english_lower = "a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z";
    font_render_text(&g_font, &g_shader_text, "English", 20.0f, sh - 140.0f, 1.0f, WHITE);
    font_render_text(&g_font, &g_shader_text, english_upper, 20.0f, sh - 180.0f, 1.0f, WHITE);
    font_render_text(&g_font, &g_shader_text, english_lower, 20.0f, sh - 220.0f, 1.0f, WHITE);

    // Français
    const char *french_upper = "A, À, Â, Æ, B, C, Ç, D, E, É, È, Ê, Ë, F, G, H, I, Î, Ï, J, K, L, M, N, O, Ô, Œ, P, Q, R, S, T, U, Ù, Û, Ü, V, W, X, Y, Ÿ, Z";
    const char *french_lower = "a, à, â, æ, b, c, ç, d, e, é, è, ê, ë, f, g, h, i, î, ï, j, k, l, m, n, o, ô, œ, p, q, r, s, t, u, ù, û, ü, v, w, x, y, ÿ, z";
    font_render_text(&g_font, &g_shader_text, "Français", 20.0f, sh - 280.0f, 1.0f, WHITE);
    font_render_text(&g_font, &g_shader_text, french_upper, 20.0f, sh - 320.0f, 1.0f, WHITE);
    font_render_text(&g_font, &g_shader_text, french_lower, 20.0f, sh - 360.0f, 1.0f, WHITE);

     // Русский
    const char *russian_upper = "А, Б, В, Г, Д, Е, Ё, Ж, З, И, Й, К, Л, М, Н, О, П, Р, С, Т, У, Ф, Х, Ц, Ч, Ш, Щ, Ъ, Ы, Ь, Э, Ю, Я";
    const char *russian_lower = "а, б, в, г, д, е, ё, ж, з, и, й, к, л, м, н, о, п, р, с, т, у, ф, х, ц, ч, ш, щ, ъ, ы, ь, э, ю, я";
    font_render_text(&g_font, &g_shader_text, "Русский", 20.0f, sh - 420.0f, 1.0f, WHITE);
    font_render_text(&g_font, &g_shader_text, russian_upper, 20.0f, sh - 460.0f, 1.0f, WHITE);
    font_render_text(&g_font, &g_shader_text, russian_lower, 20.0f, sh - 500.0f, 1.0f, WHITE);

    // Italiano
    const char *italian_upper = "A, À, B, C, D, E, È, É, F, G, H, I, Ì, Í, J, K, L, M, N, O, Ò, Ó, P, Q, R, S, T, U, Ù, Ú, V, W, X, Y, Z";
    const char *italian_lower = "a, à, b, c, d, e, è, é, f, g, h, i, ì, í, j, k, l, m, n, o, ò, ó, p, q, r, s, t, u, ù, ú, v, w, x, y, z";
    font_render_text(&g_font, &g_shader_text, "Italiano", 20.0f, sh - 560.0f, 1.0f, WHITE);
    font_render_text(&g_font, &g_shader_text, italian_upper, 20.0f, sh - 600.0f, 1.0f, WHITE);
    font_render_text(&g_font, &g_shader_text, italian_lower, 20.0f, sh - 640.0f, 1.0f, WHITE);

    // Deutsch
    const char *german_upper = "A, Ä, B, C, D, E, F, G, H, I, J, K, L, M, N, O, Ö, P, Q, R, S, ß, T, U, Ü, V, W, X, Y, Z";
    const char *german_lower = "a, ä, b, c, d, e, f, g, h, i, j, k, l, m, n, o, ö, p, q, r, s, ß, t, u, ü, v, w, x, y, z";
    font_render_text(&g_font, &g_shader_text, "Deutsch", 20.0f, sh - 700.0f, 1.0f, WHITE);
    font_render_text(&g_font, &g_shader_text, german_upper, 20.0f, sh - 740.0f, 1.0f, WHITE);
    font_render_text(&g_font, &g_shader_text, german_lower, 20.0f, sh - 780.0f, 1.0f, WHITE);

    // Polski
    const char *polish_upper = "A, Ą, B, C, Ć, D, E, Ę, F, G, H, I, J, K, L, Ł, M, N, Ń, O, Ó, P, Q, R, S, Ś, T, U, V, W, X, Y, Z, Ź, Ż";
    const char *polish_lower = "a, ą, b, c, ć, d, e, ę, f, g, h, i, j, k, l, ł, m, n, ń, o, ó, p, q, r, s, ś, t, u, v, w, x, y, z, ź, ż";
    font_render_text(&g_font, &g_shader_text, "Polski", 20.0f, sh - 840.0f, 1.0f, WHITE);
    font_render_text(&g_font, &g_shader_text, polish_upper, 20.0f, sh - 880.0f, 1.0f, WHITE);
    font_render_text(&g_font, &g_shader_text, polish_lower, 20.0f, sh - 920.0f, 1.0f, WHITE);

    // Português (Brasil)
    const char *port_upper = "A, Á, À, Â, Ã, B, C, Ç, D, E, É, Ê, F, G, H, I, Í, J, K, L, M, N, O, Ó, Ô, Õ, P, Q, R, S, T, U, Ú, V, W, X, Y, Z";
    const char *port_lower = "a, á, à, â, ã, b, c, ç, d, e, é, ê, f, g, h, i, í, j, k, l, m, n, o, ó, ô, õ, p, q, r, s, t, u, ú, v, w, x, y, z";
    font_render_text(&g_font, &g_shader_text, "Português (Brasil)", 20.0f, sh - 980.0f, 1.0f, WHITE);
    font_render_text(&g_font, &g_shader_text, port_upper, 20.0f, sh - 1020.0f, 1.0f, WHITE);
    font_render_text(&g_font, &g_shader_text, port_lower, 20.0f, sh - 1060.0f, 1.0f, WHITE);

    // Numbers
    const char *numbers = "0, 1, 2, 3, 4, 5, 6, 7, 8, 9";
    font_render_text(&g_font, &g_shader_text, "Numbers", 20.0f, sh - 1120.0f, 1.0f, WHITE);
    font_render_text(&g_font, &g_shader_text, numbers, 20.0f, sh - 1160.0f, 1.0f, WHITE);

    // Punctuations
    const char *punctuations = ". , : ; ! ? ' \" ( ) [ ] { } - — …";
    font_render_text(&g_font, &g_shader_text, "Punctuations", 20.0f, sh - 1220.0f, 1.0f, WHITE);
    font_render_text(&g_font, &g_shader_text, punctuations, 20.0f, sh - 1260.0f, 1.0f, WHITE);

    // Currency
    const char *currency = "$ € £ ¥ ₺ ₽ ₩";
    font_render_text(&g_font, &g_shader_text, "Currency", 20.0f, sh - 1320.0f, 1.0f, WHITE);
    font_render_text(&g_font, &g_shader_text, currency, 20.0f, sh - 1360.0f, 1.0f, WHITE);

    // Math
    const char *math = "+ - × ÷ = ≠ ≤ ≥ ∞ π";
    font_render_text(&g_font, &g_shader_text, "Math", 20.0f, sh - 1420.0f, 1.0f, WHITE);
    font_render_text(&g_font, &g_shader_text, math, 20.0f, sh - 1460.0f, 1.0f, WHITE);

    // Simplified Chinese
    const char *sc_basic = "的一是在不了有和人这中大为上个国我以要他时来用们生到作地于出就分对成会可主发年动同工也能下过子说产种面而方后多定行学法所民得经十三之进着等部度家电力里如水化高自二理起小物现实加量都两体制机当使点从业本去把性好应开它合还因由其些然前外天政四日那社义事平形相全表间样与关各重新线内数正心力理";
    const char *sc_numbers = "零一二三四五六七八九十百千万亿";
    const char *sc_punctuation = "，。！？；：""''（）【】《》、…—";
    font_render_text(&g_font_sc, &g_shader_text, "简体中文", 20.0f, sh - 1520.0f, 1.0f, WHITE);
    font_render_text(&g_font_sc, &g_shader_text, sc_basic, 20.0f, sh - 1560.0f, 1.0f, WHITE);
    font_render_text(&g_font_sc, &g_shader_text, sc_numbers, 20.0f, sh - 1600.0f, 1.0f, WHITE);
    font_render_text(&g_font_sc, &g_shader_text, sc_punctuation, 20.0f, sh - 1640.0f, 1.0f, WHITE);

    // Japanese
    const char *jp_hiragana = "あいうえお かきくけこ さしすせそ たちつてと なにぬねの はひふへほ まみむめも やゆよ らりるれろ わをん";
    const char *jp_katakana = "アイウエオ カキクケコ サシスセソ タチツテト ナニヌネノ ハヒフヘホ マミムメモ ヤユヨ ラリルレロ ワヲン";
    const char *jp_kanji = "日本語 山川木火水金土 一二三四五六七八九十";
    const char *jp_mixed = "私はテストです。ゲームを作っています。";
    const char *jp_punctuation = "。、！？「」『』・…";
    font_render_text(&g_font_sc, &g_shader_text, "日本語", 20.0f, sh - 1700.0f, 1.0f, WHITE);
    font_render_text(&g_font_sc, &g_shader_text, jp_hiragana, 20.0f, sh - 1740.0f, 1.0f, WHITE);
    font_render_text(&g_font_sc, &g_shader_text, jp_katakana, 20.0f, sh - 1780.0f, 1.0f, WHITE);
    font_render_text(&g_font_sc, &g_shader_text, jp_kanji, 20.0f, sh - 1820.0f, 1.0f, WHITE);
    font_render_text(&g_font_sc, &g_shader_text, jp_mixed, 20.0f, sh - 1860.0f, 1.0f, WHITE);
    font_render_text(&g_font_sc, &g_shader_text, jp_punctuation, 20.0f, sh - 1900.0f, 1.0f, WHITE);

    SDL_GL_SwapWindow(app->window);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    AppContext *app = appstate;

    font_destroy(&g_font_sc);
    font_destroy(&g_font);
    shader_destroy(&g_shader_text);

    SDL_GL_DestroyContext(app->gl_context);
    SDL_DestroyWindow(app->window);
    SDL_free(app);
}
