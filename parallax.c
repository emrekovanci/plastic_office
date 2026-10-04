#include "parallax.h"
#include <math.h>

void parallax_render(Shader *shader, int vp_w, int vp_h, float scroll,
                     Sprite *sprite, Texture *texture)
{
    const float h = (float)vp_h;
    const float w = (float)texture->width * (h / (float)texture->height);

    float offset = floorf(fmodf(scroll, w));
    if (offset < 0.0f)
        offset += w;

    for (float x = -offset; x < (float)vp_w; x += w)
        sprite_render(sprite, texture, shader, x, 0.0f, w, h, 0.0f, 0.0f, 0.0f);
}
