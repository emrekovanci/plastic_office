#ifndef PLAYER_H
#define PLAYER_H

#include "direction.h"
#include "texture.h"
#include "shader.h"
#include "input.h"
#include "camera.h"
#include "sprite.h"
#include "tilemap.h"

#define PLAYER_TILE_SIZE      128
#define PLAYER_TILE_SCALE     4
#define PLAYER_RENDER_SIZE    (PLAYER_TILE_SIZE * PLAYER_TILE_SCALE)

#define PLAYER_FRAME_TIME     (1.0f / 10.0f)

// 0.08 x 30 => 2.4 tile/secs * TILE_RENDER_SIZE(256) => 614 px/secs
#define PLAYER_MOVE_SPEED     0.08f
#define PLAYER_RUN_SPEED      0.16f

typedef struct Player Player;
struct Player {

    Sprite sprite;
    
    // Animation
    Texture texture_idle;
    Texture texture_walk;
    Texture texture_run;
    int anim_frame;
    float anim_timer;

    // Base coordinates
    int cx;                        // grid x
    int cy;                        // grid y
    float xr;                      // between 0.0f - 1.0f, represent the position inside a grid cell
    float yr;                      // between 0.0f - 1.0f, represent the position inside a grid cell

    // Resulting coordinates
    float xx;                      // exact physics pos
    float yy;                      // ""
    float prev_xx;                 // phyiscs pos before any movement applied
    float prev_yy;                 // ""
    float render_xx;               // smoothed display position, interpolated between prev and xx, always use this for rendering
    float render_yy;               // ""

    // Movements
    float dx;
    float dy;

    Direction dir;
    bool is_moving;
    bool is_running;
};

void player_init(Player *player);
void player_destroy(Player *player);

void player_on_pre_step_x(Player *player, Tilemap *tilemap);
void player_on_pre_step_y(Player *player, Tilemap *tilemap);
void player_fixed_update(Player *player, float dt, Input *input, Tilemap *tilemap);
void player_update(Player *player, float dt, Input *input, float alpha, Tilemap *tilemap);
void player_render(Player *player, Shader *shader);

// initialize cx, cy and xr, yr based on xx, yy
void player_set_coords(Player *player, float x, float y);

#endif
