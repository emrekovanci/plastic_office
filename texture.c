#include "texture.h"

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <stb_image.h>

bool texture_load(const char *path, Texture *out)
{
    stbi_set_flip_vertically_on_load(true);

    int width, height, channels;
    unsigned char *data = stbi_load(path, &width, &height, &channels, 0);
    if (!data) {
        SDL_Log("Failed to load texture: '%s': %s -- using fallback texture", path, stbi_failure_reason());
        return texture_create_solid(out, MAGENTA);
    }

    const GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    out->id = id;
    out->width = width;
    out->height = height;
    SDL_Log("Texture loaded: '%s', Size: %dx%d, Channels: %d", path, width, height, channels);

    stbi_image_free(data);
    return true;
}

void texture_destroy(Texture *tex)
{
    glDeleteTextures(1, &tex->id);
}

bool texture_create_solid(Texture *out, Color color)
{
    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);

    out->id = id;
    out->width = 1;
    out->height = 1;

    return true;
}

bool texture_create_font(Texture *out, int atlas_dim)
{
    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    unsigned char *zero = SDL_calloc(atlas_dim * atlas_dim, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlas_dim, atlas_dim, 0, GL_RED, GL_UNSIGNED_BYTE, zero);
    SDL_free(zero);

    out->id = id;
    out->width = atlas_dim;
    out->height = atlas_dim;

    return true;
}

void texture_bind(const Texture *tex, int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, tex->id);
}

void texture_unbind(int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, 0);
}
