#ifndef PARALLAX_H
#define PARALLAX_H

#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "sprite.h"

void parallax_render(Shader *shader, Camera *camera, float factor, Sprite *sprite, Texture *texture);

#endif
