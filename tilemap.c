#include <glad/glad.h>
#include "tilemap.h"

#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_log.h>
#include <cglm/cglm.h>
#include "constants.h"
#include "vertex.h"

typedef struct TilemapLayerDef TilemapLayerDef;
struct TilemapLayerDef {
    const char *csv;
    const char *texture;
};

static const TilemapLayerDef LAYER_DEFS[MAP_LAYERS] = {
    { "assets/side_Layer_1_Ground.csv",    "assets/Ground.png" },
    { "assets/side_Layer_2_Buildings.csv", "assets/Buildings.png" },
    { "assets/side_Layer_3_Decos.csv",     "assets/Decoration.png" },
    { "assets/side_Layer_4_Collision.csv", "assets/Sprite_Collision.png" },
};

static void tile_uv(int rows, int cols, int tile_id, float *u0, float *v0, float *u1, float *v1)
{
    const int col = tile_id % cols;
    const int row = (rows - 1) - (tile_id / cols);

    // to fix "texture bleeding" problem
    // this can be bad practice, idk we'll see...
    // const float eps = 0.00005f;
    //
    // *u0 = (float)col / cols + eps;
    // *v0 = (float)row / rows + eps;
    // *u1 = (float)(col + 1.0f) / cols - eps;
    // *v1 = (float)(row + 1.0f) / rows - eps;

    *u0 = (float)col / cols;
    *v0 = (float)row / rows;
    *u1 = (float)(col + 1.0f) / cols;
    *v1 = (float)(row + 1.0f) / rows;
}

static void tilemap_layer_upload(TilemapLayer *layer, Vertex *vertices, int vertex_count, unsigned int *indices, int index_count)
{
    glGenVertexArrays(1, &layer->vao);
    glGenBuffers(1, &layer->vbo);
    glGenBuffers(1, &layer->ebo);
    glBindVertexArray(layer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, layer->vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 vertex_count * sizeof(Vertex),
                 vertices,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, layer->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 index_count * sizeof(unsigned int),
                 indices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

bool tilemap_read(const char *path, int *map, int w, int h)
{
    size_t sz = 0;
    char *buf = (char*)SDL_LoadFile(path, &sz);

    if (!buf) {
        SDL_Log("Failed to load CSV file: %s", SDL_GetError());
        return false;
    }

    char *cur = buf;
    bool err = false;

    // rows are loaded bottom-to-top so row 0 corresponds to the bottom of map, matching world space.
    // this avoids needing invert_y() on lookups
    for (int r = h - 1; r >= 0 && !err; --r) {
        if (cur >= buf + sz) {
            SDL_Log("Unexpected end of CSV file at row: %d", r);
            err = true;
            break;
        }

        char *eol = SDL_strchr(cur, '\n');
        if (!eol) eol = buf + sz;
        *eol = '\0';

        char *cr = SDL_strchr(cur, '\r');
        if (cr) *cr = '\0';

        char *ctx = NULL;
        char *tok = SDL_strtok_r(cur, ",", &ctx);

        for (int c = 0; c < w; ++c) {
            if (!tok) {
                SDL_Log("Not enough columns at row %d in '%s'", r, path);
                err = true;
                break;
            }

            map[r * w + c] = SDL_atoi(tok);
            tok = SDL_strtok_r(NULL, ",", &ctx);
        }

        if (tok && !err) {
            SDL_Log("Too many columns at row %d in '%s'", r, path);
            err = true;
        }

        cur = eol + 1;
    }

    SDL_free(buf);
    return !err;
}

bool tilemap_init(Tilemap *out)
{
    Vertex *vertices = SDL_malloc(MAP_COLS * MAP_ROWS * 4 * sizeof(Vertex));
    unsigned int *indices = SDL_malloc(MAP_COLS * MAP_ROWS * 6 * sizeof(unsigned int));

    for (int l = 0; l < MAP_LAYERS; ++l) {
        TilemapLayer *layer = &out->layers[l];

        if (!texture_load(&layer->texture, LAYER_DEFS[l].texture))
            return false;

        for (int k = 0; k < MAP_COLS * MAP_ROWS; ++k)
            layer->tiles[k] = -1;

        int map[MAP_COLS * MAP_ROWS] = {0};
        if (!tilemap_read(LAYER_DEFS[l].csv, map, MAP_COLS, MAP_ROWS))
            return false;

        const int cols = layer->texture.width / TILE_SIZE;
        const int rows = layer->texture.height / TILE_SIZE;
        int v = 0;
        int i = 0;

        for (int y = 0; y < MAP_ROWS; ++y) {
            for (int x = 0; x < MAP_COLS; ++x) {
                const int tile_id = map[TILE_INDEX(x, y)];
                if (tile_id < 0)
                    continue;

                float x0 = (float)x * TILE_SIZE;
                float y0 = (float)y * TILE_SIZE;
                float x1 = x0 + TILE_SIZE;
                float y1 = y0 + TILE_SIZE;

                float u0, v0, u1, v1;
                tile_uv(rows, cols, tile_id, &u0, &v0, &u1, &v1);

                // counter-clockwise vertices and indices order
                vertices[v + 0] = (Vertex){ x0, y0, 0, 1, 1, 1, u0, v0 }; // bottom-left
                vertices[v + 1] = (Vertex){ x1, y0, 0, 1, 1, 1, u1, v0 }; // bottom-right
                vertices[v + 2] = (Vertex){ x1, y1, 0, 1, 1, 1, u1, v1 }; // top-right
                vertices[v + 3] = (Vertex){ x0, y1, 0, 1, 1, 1, u0, v1 }; // top-left
                indices[i + 0] = v + 0;
                indices[i + 1] = v + 1;
                indices[i + 2] = v + 2;
                indices[i + 3] = v + 0;
                indices[i + 4] = v + 2;
                indices[i + 5] = v + 3;

                v += 4;
                i += 6;

                layer->tiles[TILE_INDEX(x, y)] = tile_id;
            }
        }

        layer->index_count = i;
        layer->visible = (l != COLLISION_LAYER_INDEX);
        tilemap_layer_upload(layer, vertices, v, indices, i);
    }

    SDL_free(vertices);
    SDL_free(indices);

    return true;
}

void tilemap_render(Tilemap *tm, Shader *s)
{
    // TODO: no need to calculate every frame, tilemap doesnt move
    mat4 model = GLM_MAT4_IDENTITY_INIT;
    glm_translate(model, (vec3){ 0.0f, 0.0f, 0.0f });
    glm_scale(model, (vec3){ (float)TILE_SCALE, (float)TILE_SCALE, 1.0f });
    shader_set_mat4(s, "model", (float*)model);

    for (int i = 0; i < MAP_LAYERS; ++i) {
        TilemapLayer *layer = &tm->layers[i];
        if (!layer->visible)
            continue;

        texture_bind(&layer->texture, 0);
        glBindVertexArray(layer->vao);
        glDrawElements(GL_TRIANGLES, layer->index_count, GL_UNSIGNED_INT, 0);
    }
}

bool tilemap_has_collision(Tilemap *tm, int x, int y)
{
    if (x < 0 || x >= MAP_COLS || y < 0 || y >= MAP_ROWS)
        return false;

    return tm->layers[COLLISION_LAYER_INDEX].tiles[TILE_INDEX(x, y)] >= 0;
}

void tilemap_destroy(Tilemap *tm)
{
    for (int i = 0; i < MAP_LAYERS; ++i) {
        TilemapLayer *layer = &tm->layers[i];

        glDeleteVertexArrays(1, &layer->vao);
        glDeleteBuffers(1, &layer->vbo);
        glDeleteBuffers(1, &layer->ebo);
        texture_destroy(&layer->texture);
    }
}

void tilemap_toggle_collision_visibility(Tilemap *tm)
{
    TilemapLayer *collision_layer = &tm->layers[COLLISION_LAYER_INDEX];
    collision_layer->visible = !collision_layer->visible;
}
