#include "player.h"

#include <glad/glad.h>

#include "constants.h"
#include "utils.h"

static Texture *anim_texture(Player *player)
{
    if (player->is_moving && player->is_running) return &player->texture_run;
    if (player->is_moving) return &player->texture_walk;

    return &player->texture_idle;
}

static int anim_rows(Player *player)
{
    return 1;
}

static int anim_cols(Player *player)
{
    if (player->is_moving && player->is_running) return 10;
    if (player->is_moving) return 10;

    return 6;
}

void player_init(Player *player)
{
    sprite_init(&player->sprite);
    texture_load("assets/Gangsters_1/Idle.png", &player->texture_idle);
    texture_load("assets/Gangsters_1/Walk.png", &player->texture_walk);
    texture_load("assets/Gangsters_1/Run.png",  &player->texture_run);

    player->cx = MAP_COLS / 2;
    player->cy = 0;
    player->xr = 0.5f;
    player->yr = 1.0f;
    player->xx = (int)((player->cx + player->xr) * TILE_RENDER_SIZE);
    player->yy = (int)((player->cy + player->yr) * TILE_RENDER_SIZE);
    player->prev_xx = player->xx;
    player->prev_yy = player->yy;
    player->render_xx = player->xx;
    player->render_yy = player->yy;
    player->dx = 0.0f;
    player->dy = 0.0f;
    player->dir = DIRECTION_E;
    player->is_moving = false;
    player->is_running = false;
}

void player_destroy(Player *player)
{
    texture_destroy(&player->texture_run);
    texture_destroy(&player->texture_walk);
    texture_destroy(&player->texture_idle);
    sprite_destroy(&player->sprite);
}

void player_on_pre_step_x(Player *player)
{

}

void player_on_pre_step_y(Player *player)
{

}

void player_fixed_update(Player *player, float dt, Input *input, Camera *camera)
{
    // Save prev physic position for interpolation
    player->prev_xx = player->xx;
    player->prev_yy = player->yy;

    player->is_running =
        input_key_down(input, SDL_SCANCODE_LSHIFT) || input_key_down(input, SDL_SCANCODE_RSHIFT);

    player->is_moving =
        input_key_down(input, SDL_SCANCODE_A) || input_key_down(input, SDL_SCANCODE_LEFT) ||
        input_key_down(input, SDL_SCANCODE_D) || input_key_down(input, SDL_SCANCODE_RIGHT);

    float target_dx = 0.0f;
    const float speed = (player->is_running) ? PLAYER_RUN_SPEED : PLAYER_MOVE_SPEED;

    if (input_key_down(input, SDL_SCANCODE_A) || input_key_down(input, SDL_SCANCODE_LEFT)) {
        player->dir = DIRECTION_W;
        target_dx = -speed;
    }

    if (input_key_down(input, SDL_SCANCODE_D) || input_key_down(input, SDL_SCANCODE_RIGHT)) {
        player->dir = DIRECTION_E;
        target_dx = speed;
    }

    player->dx += (target_dx - player->dx) * 0.4f;

    if (fabsf(player->dx) < 0.001f) player->dx = 0.0f;
    if (fabsf(player->dy) < 0.001f) player->dy = 0.0f;

    // Stepping: any movement greater than 33% of grid size (ie. 0.33) will increase the number of `steps` here.
    // These steps will break down the full movement into smaller iterations to avoid jumping over grid collisions.
    int steps = (int)ceilf((fabsf(player->dx) + fabsf(player->dy)) / 0.33f);
    if (steps > 0) {
        int n = 0;
        while (n < steps) {
            player->xr += player->dx / steps;

            if (player->dx != 0.0f)
                player_on_pre_step_x(player);      // add x collision checks and physics in here

            while (player->xr > 1) { player->xr--; player->cx++; }
            while (player->xr < 0) { player->xr++; player->cx--; }

            player->yr += player->dy / steps;

            if (player->dy != 0.0f)
                player_on_pre_step_y(player);     // add y collision checks and physics in here

            while (player->yr > 1) { player->yr--; player->cy++; }
            while (player->yr < 0) { player->yr++; player->cy--; }

            ++n;
        }
    }

    player->xx = (player->cx + player->xr) * TILE_RENDER_SIZE;
    player->yy = (player->cy + player->yr) * TILE_RENDER_SIZE;

    if (!player->is_moving)
        player->is_moving = fabsf(player->dx) > 0.01f;
}

void player_update(Player *player, float dt, Input *input, Camera *camera, float alpha)
{
    player->anim_timer += dt;
    if (player->anim_timer >= PLAYER_FRAME_TIME) {
        player->anim_timer -= PLAYER_FRAME_TIME;
        player->anim_frame = (player->anim_frame + 1) % anim_cols(player);
    }

    player->render_xx = lerpf(player->prev_xx, player->xx, alpha);
    player->render_yy = lerpf(player->prev_yy, player->yy, alpha);
}

void player_render(Player *player, Shader *shader)
{
    // There is only one row in current animation tilesheets
    // This should be player's direction if there are multiple rows
    const int dir_row = 0;

    float u0, v0, u1, v1;
    tile_uv(anim_rows(player), anim_cols(player), dir_row * anim_cols(player) + player->anim_frame, &u0, &v0, &u1, &v1);

    // Flip UV when direction change
    if (player->dir == DIRECTION_W)
        swapf(&u0, &u1);

    Sprite *sprite = &player->sprite;
    sprite->vertices[0].u = u1;
    sprite->vertices[0].v = v1;
    sprite->vertices[1].u = u1;
    sprite->vertices[1].v = v0;
    sprite->vertices[2].u = u0;
    sprite->vertices[2].v = v0;
    sprite->vertices[3].u = u0;
    sprite->vertices[3].v = v1;

    glBindBuffer(GL_ARRAY_BUFFER, sprite->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sprite->vertices), sprite->vertices);

    sprite_render(
        &player->sprite,
        anim_texture(player),
        shader,
        player->render_xx, player->render_yy,
        PLAYER_RENDER_SIZE, PLAYER_RENDER_SIZE,
        0.0f);
}

void player_set_coords(Player *player, float x, float y)
{
    player->xx = x;
    player->yy = y;
    player->cx = (int)(player->xx / TILE_RENDER_SIZE);
    player->cy = (int)(player->yy / TILE_RENDER_SIZE);
    player->xr = (player->xx - player->cx * TILE_RENDER_SIZE) / TILE_RENDER_SIZE;
    player->yr = (player->yy - player->cy * TILE_RENDER_SIZE) / TILE_RENDER_SIZE;
}
