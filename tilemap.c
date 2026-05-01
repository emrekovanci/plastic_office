#include "tilemap.h"

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include "utils.h"

static const char *layer_csvs[MAP_LAYERS] = {
    "assets/side_Layer_1_Ground.csv",
    "assets/side_Layer_2_Buildings.csv",
    "assets/side_Layer_3_Decos.csv"
};

static const char *layer_textures[MAP_LAYERS] = {
    "assets/Ground.png",
    "assets/Buildings.png",
    "assets/Decoration.png"
};

static const char *layer_collision = "assets/side_Layer_4_Collision.csv";

static void tilemap_layer_upload(TilemapLayer *layer)
{
    glGenVertexArrays(1, &layer->vao);
    glGenBuffers(1, &layer->vbo);
    glGenBuffers(1, &layer->ebo);

    glBindVertexArray(layer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, layer->vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 layer->vertex_count * sizeof(Vertex),
                 layer->vertices,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, layer->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 layer->index_count * sizeof(unsigned int),
                 layer->indices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

// Rows are loaded bottom-to-top so row 0 corresponds to the bottom of map,
// matching world space. This avoids needing invert_y() on lookups.
bool tilemap_read(const char *path, int out[MAP_ROWS][MAP_COLS])
{
    FILE *fp = fopen(path, "r");
    if (!fp) {
        SDL_Log("Failed to open csv file: %s", path);
        return false;
    }

    int tmap[MAP_ROWS][MAP_COLS] = {0};
    int row = MAP_ROWS - 1;
    int col = 0;

    char line[MAP_COLS * 4 + 2] = {0};
    while (fgets(line, sizeof(line), fp)) {
        col = 0;
        char *ctx = NULL;
        char *pc = SDL_strtok_r(line, ",", &ctx);
        while (pc) {
            tmap[row][col++] = SDL_atoi(pc);
            pc = SDL_strtok_r(NULL, ",", &ctx);
        }
        --row;
    }
    fclose(fp);

    SDL_memcpy(out, tmap, sizeof(tmap));
    return true;
}

bool tilemap_build(Tilemap *out)
{
    for (int l = 0; l < MAP_LAYERS; ++l) {
        TilemapLayer *layer = &out->layers[l];

        if (!texture_load(layer_textures[l], &layer->texture))
            return false;

        int map[MAP_ROWS][MAP_COLS] = {0};
        if (!tilemap_read(layer_csvs[l], map))
            return false;

        const int cols = layer->texture.width / TILE_SIZE;
        const int rows = layer->texture.height / TILE_SIZE;
        int v = 0;
        int i = 0;

        for (int y = 0; y < MAP_ROWS; ++y) {
            for (int x = 0; x < MAP_COLS; ++x) {
                const int tile_id = map[y][x];
                if (tile_id < 0)
                    continue;

                float x0 = (float)x * TILE_SIZE;
                float y0 = (float)y * TILE_SIZE;
                float x1 = x0 + TILE_SIZE;
                float y1 = y0 + TILE_SIZE;

                float u0, v0, u1, v1;
                tile_uv(rows, cols, tile_id, &u0, &v0, &u1, &v1);

                layer->vertices[v + 0] = (Vertex){ x1, y1, 0, 1, 1, 1, u1, v1 }; // top-right
                layer->vertices[v + 1] = (Vertex){ x1, y0, 0, 1, 1, 1, u1, v0 }; // bottom-right
                layer->vertices[v + 2] = (Vertex){ x0, y0, 0, 1, 1, 1, u0, v0 }; // bottom-left
                layer->vertices[v + 3] = (Vertex){ x0, y1, 0, 1, 1, 1, u0, v1 }; // top-left
                layer->indices[i + 0] = v + 0;
                layer->indices[i + 1] = v + 1;
                layer->indices[i + 2] = v + 3;
                layer->indices[i + 3] = v + 1;
                layer->indices[i + 4] = v + 2;
                layer->indices[i + 5] = v + 3;

                v += 4;
                i += 6;
            }
        }

        layer->vertex_count = v;
        layer->index_count = i;
        tilemap_layer_upload(layer);
    }

    if (!tilemap_read(layer_collision, out->collision))
        return false;

    return true;
}

void tilemap_render(Tilemap *tilemap, Shader *shader)
{
    // TODO: no need to calculate every frame, tilemap doesnt move
    mat4 model = GLM_MAT4_IDENTITY_INIT;
    glm_translate(model, (vec3){ 0.0f, 0.0f, 0.0f });
    glm_scale(model, (vec3){ (float)TILE_SCALE, (float)TILE_SCALE, 1.0f });
    shader_set_mat4(shader, "model", (float*)model);

    for (int i = 0; i < MAP_LAYERS; ++i) {
        TilemapLayer *layer = &tilemap->layers[i];
        texture_bind(&layer->texture, 0);
        glBindVertexArray(layer->vao);
        glDrawElements(GL_TRIANGLES, layer->index_count, GL_UNSIGNED_INT, 0);
    }
}

bool tilemap_has_collision(Tilemap *tilemap, int x, int y)
{
    return tilemap->collision[y][x] > 0;
}

void tilemap_destroy(Tilemap *tilemap)
{
    for (int i = 0; i < MAP_LAYERS; ++i) {
        TilemapLayer *layer = &tilemap->layers[i];
        glDeleteVertexArrays(1, &layer->vao);
        glDeleteBuffers(1, &layer->vbo);
        glDeleteBuffers(1, &layer->ebo);
        texture_destroy(&layer->texture);
    }
}
