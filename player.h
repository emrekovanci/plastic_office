#ifndef PLAYER_H
#define PLAYER_H

#include "constants.h"
#include "texture.h"
#include "shader.h"
#include "input.h"
#include "sprite.h"
#include "tilemap.h"

#define PLAYER_SHEET_COLS         4
#define PLAYER_SHEET_ROWS         6
#define PLAYER_RENDER_W           16
#define PLAYER_RENDER_H           32

#define PLAYER_HITBOX_HALF_WIDTH  0.5f
#define PLAYER_HITBOX_HEIGHT      2.0f

#define PLAYER_MOVE_SPEED         0.25f
#define PLAYER_RUN_SPEED          0.5f
#define PLAYER_GRAVITY           -0.08f
#define PLAYER_JUMP_FORCE         1.1f
#define PLAYER_EPSILON            0.001f

typedef enum AnimID {
    ANIM_IDLE,
    ANIM_WALK,
    ANIM_JUMP,
    ANIM_COUNT
} AnimID;

typedef struct Animation {
    int row;
    int col;
    int frames;
    float frame_time;
} Animation;

static const Animation PLAYER_ANIMS[ANIM_COUNT] = {
    [ANIM_IDLE] = { .row = 0, .col = 0, .frames = 1, .frame_time = 1.0f / 12.0f },
    [ANIM_WALK] = { .row = 1, .col = 0, .frames = 4, .frame_time = 1.0f / 10.0f },
    [ANIM_JUMP] = { .row = 2, .col = 0, .frames = 2, .frame_time = 1.0f / 12.0f },
};

typedef struct Player {

    Sprite sprite;
    Texture texture_hitbox;        // debugging

    // animation
    Texture texture_sheet;
    AnimID anim_id;
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

    bool jump_requested;

    Direction dir;
    bool is_moving;
    bool is_running;
    bool grounded;

    // debugging
    bool render_hitbox;
} Player;

void player_init(Player *player);
void player_destroy(Player *player);

void player_fixed_update(Player *player, float dt, Input *input, Tilemap *tilemap);
void player_update(Player *player, float dt, Input *input, float alpha, Tilemap *tilemap);
void player_render(Player *player, Shader *shader);

#endif
