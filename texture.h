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

bool texture_create_solid(Texture *out, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

void texture_bind(const Texture *tex, int slot);
void texture_unbind(int slot);

#endif
