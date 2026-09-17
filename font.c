#include <glad/glad.h>
#include "font.h"

#include <SDL3/SDL.h>

// ----------------------------------------------------------------------------
// leading byte patterns:
//   1 byte => 0xxxxxxx
//   2 byte => 110xxxxx
//   3 byte => 1110xxxx
//   4 byte => 11110xxx
// continuation byte pattern: 10xxxxxx  range: 0x80(10000000) - 0xBF(10111111)
// ----------------------------------------------------------------------------
// full encoding:
//   1 byte => 0yyyzzzz
//   2 byte => 110xxxyy 10yyzzzz
//   3 byte => 1110wwww 10xxxxyy 10yyzzzz
//   4 byte => 11110uvv 10vvwwww 10xxxxyy 10yyzzzz
// ----------------------------------------------------------------------------
static int utf8_decode(const char *p, uint32_t *codepoint)
{
    const unsigned char *s = (const unsigned char *)p;
    if (s[0] < 0x80) {
        *codepoint = s[0];
        return 1;
    } else if ((s[0] & 0xE0) == 0xC0) {
        *codepoint = (s[0] & 0x1F) << 6 | (s[1] & 0x3F);
        return 2;
    } else if ((s[0] & 0xF0) == 0xE0) {
        *codepoint = (s[0] & 0x0F) << 12 | (s[1] & 0x3F) << 6 | (s[2] & 0x3F);
        return 3;
    } else if ((s[0] & 0xF8) == 0xF0) {
        *codepoint = (s[0] & 0x07) << 18 | (s[1] & 0x3F) << 12 | (s[2] & 0x3F) << 6 | (s[3] & 0x3F);
        return 4;
    }

    *codepoint = 0xFFFD; // replacement character
    return 1;
}

static Glyph *font_get_glyph(Font *font, uint32_t codepoint)
{
    ptrdiff_t idx = stbds_hmgeti(font->glyph_cache, codepoint);
    if (idx >= 0)
        return &font->glyph_cache[idx].value;

    if (FT_Load_Char(font->face, codepoint, FT_LOAD_RENDER)) {
        SDL_Log("Failed to load glyph: U+%04X", codepoint);
        return NULL;
    }

    FT_GlyphSlot gs = font->face->glyph;
    const int w = gs->bitmap.width;
    const int h = gs->bitmap.rows;

    if (font->cursor_x + w + FONT_ATLAS_PADDING > font->atlas.width) {
        font->cursor_x = 0;
        font->cursor_y += font->row_height + FONT_ATLAS_PADDING;
        font->row_height = 0;
    }

    if (font->cursor_y + h > font->atlas.height) {
        SDL_Log("Font atlas is full! Codepoint: U+%04X", codepoint);
        return NULL;
    }

    const int x = font->cursor_x;
    const int y = font->cursor_y;

    if (w > 0 && h > 0) {
        // atlas is always bound in font_render_text before any glyph lookup
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_RED, GL_UNSIGNED_BYTE, gs->bitmap.buffer);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }

    font->cursor_x += w + FONT_ATLAS_PADDING;
    if (h > font->row_height)
        font->row_height = h;

    Glyph glyph = {
        .u0 = (float)x / (float)font->atlas.width,
        .v0 = (float)y / (float)font->atlas.height,
        .u1 = (float)(x + w) / (float)font->atlas.width,
        .v1 = (float)(y + h) / (float)font->atlas.height,
        .width = w,
        .height = h,
        .bearing_x = gs->bitmap_left,
        .bearing_y = gs->bitmap_top,
        .advance = gs->advance.x
    };
    stbds_hmput(font->glyph_cache, codepoint, glyph);

    // hmput may realloc, re-fetch index to get a valid pointer
    idx = stbds_hmgeti(font->glyph_cache, codepoint);
    return &font->glyph_cache[idx].value;
}

bool font_load(Font *font, const char *path, unsigned int size, int atlas_dim)
{
    if (FT_Init_FreeType(&font->ft)) {
        SDL_Log("Failed to initialize FreeType!");
        return false;
    }

    if (FT_New_Face(font->ft, path, 0, &font->face)) {
        SDL_Log("Failed to load font: %s", path);
        FT_Done_FreeType(font->ft);
        return false;
    }

    // font props
    FT_Set_Pixel_Sizes(font->face, 0, size);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    font->glyph_cache = NULL;
    font->cursor_x = 0;
    font->cursor_y = 0;
    font->row_height = 0;
    font->kerning = FT_HAS_KERNING(font->face);
    texture_create_font(&font->atlas, atlas_dim);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    // font atlas quad
    glGenVertexArrays(1, &font->vao);
    glGenBuffers(1, &font->vbo);
    glBindVertexArray(font->vao);
    glBindBuffer(GL_ARRAY_BUFFER, font->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4 * FONT_BATCH_SIZE, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    SDL_Log("Font loaded: %s, Size: %u, Atlas: %dx%d, Has Kerning: %d", path, size, atlas_dim, atlas_dim, font->kerning);
    return true;
}

void font_destroy(Font *font)
{
    stbds_hmfree(font->glyph_cache);
    texture_destroy(&font->atlas);
    glDeleteVertexArrays(1, &font->vao);
    glDeleteBuffers(1, &font->vbo);
    FT_Done_Face(font->face);
    FT_Done_FreeType(font->ft);
}

void font_render_text(Font *font, Shader *shader, const char *text, float x, float y, float scale, Color color)
{
    shader_bind(shader);
    shader_set_rgb(shader, "textColor", color);
    texture_bind(&font->atlas, 0);
    glBindVertexArray(font->vao);
    glBindBuffer(GL_ARRAY_BUFFER, font->vbo);

    float verts[6 * 4 * FONT_BATCH_SIZE];
    int vc = 0;

    const char *p = text;
    uint32_t prev_cp = 0;

    while (*p) {
        uint32_t cp;
        p += utf8_decode(p, &cp);

        Glyph *gl = font_get_glyph(font, cp);
        if (!gl) {
            prev_cp = 0;
            continue;
        }

        // kerning - https://freetype.org/freetype2/docs/tutorial/step2.html (4.a)
        if (font->kerning && prev_cp) {
            FT_Vector delta;
            FT_UInt prev_idx = FT_Get_Char_Index(font->face, prev_cp);
            FT_UInt curr_idx = FT_Get_Char_Index(font->face, cp);
            FT_Get_Kerning(font->face, prev_idx, curr_idx, FT_KERNING_DEFAULT, &delta);
            x += (delta.x >> 6) * scale;
        }
        prev_cp = cp;

        const float xpos = x + gl->bearing_x * scale;
        const float ypos = y - (gl->height - gl->bearing_y) * scale;
        const float w = gl->width  * scale;
        const float h = gl->height * scale;

        // flip v0/v1 to correct y-axis mismatch
        const float quad[6][4] = {
            { xpos,     ypos,     gl->u0, gl->v1 }, // bottom-left
            { xpos + w, ypos,     gl->u1, gl->v1 }, // bottom-right
            { xpos + w, ypos + h, gl->u1, gl->v0 }, // top-right

            { xpos,     ypos,     gl->u0, gl->v1 }, // bottom-left
            { xpos + w, ypos + h, gl->u1, gl->v0 }, // top-right
            { xpos,     ypos + h, gl->u0, gl->v0 }  // top-left
        };

        SDL_memcpy(&verts[vc * 4], quad, sizeof(quad));
        vc += 6;

        if (vc >= 6 * FONT_BATCH_SIZE) {
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 4 * vc, verts);
            glDrawArrays(GL_TRIANGLES, 0, vc);
            vc = 0;
        }

        x += (gl->advance >> 6) * scale;
    }

    if (vc > 0) {
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 4 * vc, verts);
        glDrawArrays(GL_TRIANGLES, 0, vc);
    }
}
