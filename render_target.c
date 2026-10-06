#include <glad/glad.h>
#include "render_target.h"

#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_log.h>
#include <math.h>

bool rt_create(RenderTarget *rt, int w, int h)
{
    rt->w = w;
    rt->h = h;

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
    glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

RTLayout rt_layout(const RenderTarget *rt, int win_w, int win_h, float sub_x, float sub_y)
{
    const int lw = rt->w - 2 * RT_MARGIN;
    const int lh = rt->h - 2 * RT_MARGIN;

    RTLayout L;
    
    L.scale = SDL_max(1, SDL_min(win_w / lw, win_h / lh));

    L.lr.w = lw * L.scale;
    L.lr.h = lh * L.scale;
    L.lr.x = (win_w - L.lr.w) / 2;
    L.lr.y = (win_h - L.lr.h) / 2;

    L.vr.w = rt->w * L.scale;
    L.vr.h = rt->h * L.scale;
    L.vr.x = L.lr.x - RT_MARGIN * L.scale + (int)roundf(-sub_x * L.scale);
    L.vr.y = L.lr.y - RT_MARGIN * L.scale + (int)roundf(-sub_y * L.scale);
    
    return L;
}

void rt_present(const RenderTarget *rt, int win_w, int win_h, const RTLayout *L)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glViewport(0, 0, win_w, win_h);
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // black bars
    glClear(GL_COLOR_BUFFER_BIT);

    // Clip to the visible area so the margin texels and the shifted edges
    // never leak into the black bars
    glEnable(GL_SCISSOR_TEST);
    glScissor(L->lr.x, L->lr.y, L->lr.w, L->lr.h);

    glBlitFramebuffer(0, 0, rt->w, rt->h,
                      L->vr.x, L->vr.y, L->vr.x + L->vr.w, L->vr.y + L->vr.h,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glDisable(GL_SCISSOR_TEST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}
