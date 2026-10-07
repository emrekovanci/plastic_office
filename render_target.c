#include <glad/glad.h>
#include "render_target.h"

#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_log.h>
#include <math.h>

#define COLOR_BG         0.0f, 0.0f, 0.0f, 1.0f
#define COLOR_BLACK_BARS 0.0f, 0.0f, 1.0f, 1.0f

bool rt_create(RenderTarget *rt, int win_w, int win_h, int w, int h)
{
    rt->w = w;
    rt->h = h;
    rt->scale = (int)fminf((float)win_w / rt->w, (float)win_h / rt->h);

    glGenTextures(1, &rt->color);
    glBindTexture(GL_TEXTURE_2D, rt->color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &rt->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, rt->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt->color, 0);

    bool ok = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return ok;
}

void rt_destroy(RenderTarget *rt)
{
    glDeleteFramebuffers(1, &rt->fbo);
    glDeleteTextures(1, &rt->color);
}

void rt_begin(const RenderTarget *rt)
{
    glBindFramebuffer(GL_FRAMEBUFFER, rt->fbo);
    glViewport(0, 0, rt->w, rt->h);
    glClearColor(COLOR_BG);
    glClear(GL_COLOR_BUFFER_BIT);
}

void rt_present(const RenderTarget *rt, int win_w, int win_h, const Camera *cam)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, win_w, win_h);
    glClearColor(COLOR_BLACK_BARS);
    glClear(GL_COLOR_BUFFER_BIT);

    int render_w = (int)(rt->w * rt->scale);
    int render_h = (int)(rt->h * rt->scale);
    int render_x = (win_w - render_w) / 2;
    int render_y = (win_h - render_h) / 2;

    glBindFramebuffer(GL_READ_FRAMEBUFFER, rt->fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    int sub_x_scaled = (int)(cam->sub_x * rt->scale);
    int sub_y_scaled = (int)(cam->sub_y * rt->scale);

    glBlitFramebuffer(
        0, 0, rt->w, rt->h,
        render_x + sub_x_scaled, render_y + sub_y_scaled, render_x + render_w + sub_x_scaled, render_y + render_h + sub_y_scaled,
        GL_COLOR_BUFFER_BIT, 
        GL_NEAREST
    );

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}
