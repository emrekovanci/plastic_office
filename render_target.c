#include <glad/glad.h>
#include "render_target.h"

#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_log.h>
#include <math.h>

#define COLOR_BG         0.0f, 0.0f, 0.0f, 1.0f
#define COLOR_BLACK_BARS 0.0f, 0.0f, 1.0f, 1.0f

bool rt_create(RenderTarget *rt, int w, int h)
{
    rt->w = w;
    rt->h = h;

    glGenTextures(1, &rt->color);
    glBindTexture(GL_TEXTURE_2D, rt->color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
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

void rt_present(const RenderTarget *rt, int win_w, int win_h)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, win_w, win_h);
    glClearColor(COLOR_BLACK_BARS);
    glClear(GL_COLOR_BUFFER_BIT);

    int scale = (int)fminf((float)win_w / rt->w, (float)win_h / rt->h);
    if (scale < 1)
        scale = 1;

    int render_w = (int)(rt->w * scale);
    int render_h = (int)(rt->h * scale);
    int render_x = (win_w - render_w) / 2;
    int render_y = (win_h - render_h) / 2;

    glBindFramebuffer(GL_READ_FRAMEBUFFER, rt->fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glBlitFramebuffer(
        0, 0, rt->w, rt->h,
        render_x, render_y, render_x + render_w, render_y + render_h,
        GL_COLOR_BUFFER_BIT, 
        GL_NEAREST
    );

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}
