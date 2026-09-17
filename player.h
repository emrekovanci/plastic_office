#ifndef PLAYER_H
#define PLAYER_H

#include "constants.h"
#include "texture.h"
#include "shader.h"
#include "input.h"
#include "sprite.h"
#include "tilemap.h"

#define PLAYER_TILE_SIZE          128
#define PLAYER_TILE_SCALE         5
#define PLAYER_RENDER_SIZE        (PLAYER_TILE_SIZE * PLAYER_TILE_SCALE)

// both of them in tiles
#define PLAYER_HITBOX_HALF_WIDTH  0.3f
#define PLAYER_HITBOX_HEIGHT      1.5f

// animation
#define PLAYER_FRAME_TIME         (1.0f / 10.0f)

// movement
// 0.08 x 30 => 2.4 tile/secs * TILE_RENDER_SIZE(256) => 614 px/secs
#define PLAYER_MOVE_SPEED         0.08f
#define PLAYER_RUN_SPEED          0.16f
#define PLAYER_GRAVITY           -0.05f
#define PLAYER_JUMP_FORCE         0.67f
#define PLAYER_JUMP_BUFFER        0.18f
#define PLAYER_EPSILON            0.001f

typedef struct Player Player;
struct Player {

    Sprite sprite;

    // animation
    Texture texture_idle;
    Texture texture_walk;
    Texture texture_run;
    Texture texture_jump;
    Texture texture_hitbox;        // debugging
    int anim_frame;
    float anim_timer;

    // base coordinates
    int cx;                        // grid x
    int cy;                        // grid y
    float xr;                      // between 0.0f - 1.0f, represent the position inside a grid cell
    float yr;                      // between 0.0f - 1.0f, represent the position inside a grid cell

    // resulting coordinates
    float xx;                      // exact physics pos
    float yy;                      // ""
    float prev_xx;                 // phyiscs pos before any movement applied
    float prev_yy;                 // ""
    float render_xx;               // smoothed display position, interpolated between prev and xx, always use this for rendering
    float render_yy;               // ""

    // movements
    float dx;
    float dy;

    float last_axis;
    float axis_hold_timer;
    float jump_buffer_timer;
    float landing_timer;

    Direction dir;
    bool is_moving;
    bool is_running;
    bool grounded;
    bool prev_grounded;
    bool is_landing;

    // debugging
    bool render_hitbox;
};

void player_init(Player *player);
void player_destroy(Player *player);

void player_fixed_update(Player *player, float dt, Input *input, Tilemap *tilemap);
void player_update(Player *player, float dt, Input *input, float alpha, Tilemap *tilemap);
void player_render(Player *player, Shader *shader);

#endif
