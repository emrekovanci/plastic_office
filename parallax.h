#ifndef PARALLAX_H
#define PARALLAX_H

#include "shader.h"
#include "texture.h"
#include "sprite.h"

void parallax_render(Shader *shader, int vp_w, int vp_h, float scroll,
                     Sprite *sprite, Texture *texture);

#endif
