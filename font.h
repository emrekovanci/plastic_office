#ifndef FONT_H
#define FONT_H

#include "texture.h"
#include "shader.h"
#include "color.h"
#include <stb_ds.h>
#include <stdint.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#define FONT_BATCH_SIZE    256
#define FONT_ATLAS_PADDING 1

typedef struct Glyph Glyph;
struct Glyph {
    float u0, v0;
    float u1, v1;
    int width, height;
    int bearing_x, bearing_y;
    long advance;
};

typedef struct GlyphEntry GlyphEntry;
struct GlyphEntry {
    uint32_t key; // codepoint
    Glyph value;  // glyph
};

typedef struct Font Font;
struct Font {
    GlyphEntry *glyph_cache;
    Texture atlas;
    unsigned int vao;
    unsigned int vbo;
    int cursor_x;
    int cursor_y;
    int row_height;
    bool kerning;
    FT_Library ft;
    FT_Face face;
};

bool font_load(Font *font, const char *path, unsigned int size, int atlas_dim);
void font_destroy(Font *font);
void font_render_text(Font *font, Shader *shader, const char *text, float x, float y, float scale, Color color);

#endif
