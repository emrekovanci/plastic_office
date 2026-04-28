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
        SDL_Log("Failed to load texture: '%s': %s", path, stbi_failure_reason());
        return false;
    }

    const GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    out->id = id;
    out->width = width;
    out->height = height;
    SDL_Log("Texture '%s' loaded: %dx%d ch:%d", path, width, height, channels);

    return true;
}

void texture_destroy(Texture *tex)
{
    glDeleteTextures(1, &tex->id);
    tex->id = 0;
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
