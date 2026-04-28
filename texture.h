#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdbool.h>

typedef struct Texture Texture;
struct Texture {
    unsigned int id;
    int width;
    int height;
};

bool texture_load(const char *path, Texture *out);
void texture_destroy(Texture *tex);

void texture_bind(const Texture *tex, int slot);
void texture_unbind(int slot);

#endif
