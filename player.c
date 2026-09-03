#include "player.h"
#include <math.h>
#include "constants.h"
#include "utils.h"

// hitbox rendering for debugging purposes
static bool g_render_hitbox = false;

static void player_render_hitbox(Player *player, Shader *shader)
{
    const float width = PLAYER_HITBOX_HALF_WIDTH * 2.0f * TILE_RENDER_SIZE;
    const float height = PLAYER_HITBOX_HEIGHT * TILE_RENDER_SIZE;

    shader_set_vec2(shader, "uv_scale",  1.0f, 1.0f);
    shader_set_vec2(shader, "uv_offset", 0.0f, 0.0f);

    sprite_render(
        &player->sprite,
        &player->texture_hitbox,
        shader,
        player->render_xx, player->render_yy,
        width, height,
        0.0f,
        0.5f, 0.0f);
}

static bool collides_x(Tilemap *tilemap, int tile_x, int y_min, int y_max)
{
    for (int y = y_min; y <= y_max; ++y)
        if (tilemap_has_collision(tilemap, tile_x, y))
            return true;

    return false;
}

static bool collides_y(Tilemap *tilemap, int tile_y, int x_min, int x_max)
{
    for (int x = x_min; x <= x_max; ++x)
        if (tilemap_has_collision(tilemap, x, tile_y))
            return true;

    return false;
}

static Texture *anim_texture(Player *player)
{
    if (!player->grounded || player->is_landing) return &player->texture_jump;
    if (player->is_moving && player->is_running) return &player->texture_run;
    if (player->is_moving) return &player->texture_walk;
    return &player->texture_idle;
}

static int anim_rows(Player *player)
{
    const Texture *tex = anim_texture(player);
    return tex->height / PLAYER_TILE_SIZE;
}

static int anim_cols(Player *player)
{
    const Texture *tex = anim_texture(player);
    return tex->width / PLAYER_TILE_SIZE;
}

static bool is_grounded(Player *player, Tilemap *tilemap)
{
    if (player->yr > 0.01f)
        return false;

    int x_min = (int)floorf(player->cx + player->xr - PLAYER_HITBOX_HALF_WIDTH + 0.01f);
    int x_max = (int)floorf(player->cx + player->xr + PLAYER_HITBOX_HALF_WIDTH - 0.01f);

    return collides_y(tilemap, player->cy - 1, x_min, x_max);
}

static void player_on_pre_step_x(Player *player, Tilemap *tilemap)
{
    const int y_min = (int)floorf(player->cy + player->yr);
    const int y_max = (int)floorf(player->cy + player->yr + PLAYER_HITBOX_HEIGHT - 0.01f);

    // right collision
    // snap the right edge of the hitbox to the left edge of the right tile
    const float right_x = player->cx + player->xr + PLAYER_HITBOX_HALF_WIDTH;
    const int right_tile = (int)floorf(right_x);
    if (right_x > right_tile) {
        if (collides_x(tilemap, right_tile, y_min, y_max)) {
            player->xr = right_tile - player->cx - PLAYER_HITBOX_HALF_WIDTH;
            player->dx = 0.0f;
        }
    }

    // left collision
    // snap the left edge of the hitbox to the right edge of the left tile
    const float left_x = player->cx + player->xr - PLAYER_HITBOX_HALF_WIDTH;
    const int left_tile = (int)floorf(left_x);
    if (left_x < left_tile + 1) {
        if (collides_x(tilemap, left_tile, y_min, y_max)) {
            player->xr = (left_tile + 1) - player->cx + PLAYER_HITBOX_HALF_WIDTH;
            player->dx = 0.0f;
        }
    }
}

static void player_on_pre_step_y(Player *player, Tilemap *tilemap)
{
    const int x_min = (int)floorf(player->cx + player->xr - PLAYER_HITBOX_HALF_WIDTH + 0.01f);
    const int x_max = (int)floorf(player->cx + player->xr + PLAYER_HITBOX_HALF_WIDTH - 0.01f);

    // ground collision
    if (player->yr < 0.0f) {
        if (collides_y(tilemap, player->cy - 1, x_min, x_max)) {
            player->yr = 0.0f;
            player->dy = 0.0f;
        }
    }

    // ceiling collision
    const float top_y = player->cy + player->yr + PLAYER_HITBOX_HEIGHT;
    const int top_tile = (int)floorf(top_y);
    if (top_y > top_tile) {
        if (collides_y(tilemap, top_tile, x_min, x_max)) {
            player->yr = top_tile - player->cy - PLAYER_HITBOX_HEIGHT;
            if (player->dy > 0.0f)
                player->dy = 0.0f;
        }
    }
}

void player_init(Player *player)
{
    sprite_create(&player->sprite);
    texture_load("assets/Gangsters_1/Idle.png", &player->texture_idle);
    texture_load("assets/Gangsters_1/Walk.png", &player->texture_walk);
    texture_load("assets/Gangsters_1/Run.png", &player->texture_run);
    texture_load("assets/Gangsters_1/Jump.png", &player->texture_jump);
    texture_create_solid(&player->texture_hitbox, (Color){ 255, 0, 0, 100 });

    player->anim_frame = 0;
    player->anim_timer = 0.0f;
    player->cx = 2;
    player->cy = 5;
    player->xr = 0.5f;
    player->yr = 0.0f;
    player->xx = (player->cx + player->xr) * TILE_RENDER_SIZE;
    player->yy = (player->cy + player->yr) * TILE_RENDER_SIZE;
    player->prev_xx = player->xx;
    player->prev_yy = player->yy;
    player->render_xx = player->xx;
    player->render_yy = player->yy;
    player->dx = 0.0f;
    player->dy = 0.0f;
    player->last_axis = 0.0f;
    player->axis_hold_timer = 0.0f;
    player->jump_buffer_timer = 0.0f;
    player->landing_timer = 0.0f;
    player->dir = DIRECTION_E;
    player->is_moving = false;
    player->is_running = false;
    player->grounded = false;
    player->prev_grounded = false;
    player->is_landing = false;
}

void player_destroy(Player *player)
{
    texture_destroy(&player->texture_hitbox);
    texture_destroy(&player->texture_jump);
    texture_destroy(&player->texture_run);
    texture_destroy(&player->texture_walk);
    texture_destroy(&player->texture_idle);
    sprite_destroy(&player->sprite);
}

void player_fixed_update(Player *player, float dt, Input *input, Tilemap *tilemap)
{
    // save prev physic state
    player->prev_xx = player->xx;
    player->prev_yy = player->yy;
    player->prev_grounded = player->grounded;

    // read sustained inputs
    const bool left_down = input_key_down(input, SDL_SCANCODE_A) || input_key_down(input, SDL_SCANCODE_LEFT);
    const bool right_down = input_key_down(input, SDL_SCANCODE_D) || input_key_down(input, SDL_SCANCODE_RIGHT);
    const bool shift_down = input_key_down(input, SDL_SCANCODE_LSHIFT) || input_key_down(input, SDL_SCANCODE_RSHIFT);

    const float raw_axis = right_down * 2.0f - left_down;
    float axis = signf(raw_axis);

    if (axis != 0.0f) {
        player->last_axis = axis;
        player->axis_hold_timer = 0.05f;
    } else {
        player->axis_hold_timer -= dt;
        if (player->axis_hold_timer > 0.0f)
            axis = player->last_axis;
    }

    const bool moving = (axis != 0.0f);
    const bool running = moving && shift_down;

    const float speed = running ? PLAYER_RUN_SPEED : PLAYER_MOVE_SPEED;
    const float target_dx = speed * signf(axis);

    const bool direction_changed =
        (target_dx > PLAYER_EPSILON && player->dx < -PLAYER_EPSILON) ||
        (target_dx < -PLAYER_EPSILON && player->dx > PLAYER_EPSILON);

    const bool speed_changed = ((player->is_running != running) && moving) || (player->is_moving != moving);

    if (direction_changed || speed_changed) {
        player->anim_frame = 0;
        player->anim_timer = 0.0f;
    }

    if (target_dx > 0.0f)
        player->dir = DIRECTION_E;
    else if (target_dx < 0.0f)
        player->dir = DIRECTION_W;

    player->is_moving = moving;
    player->is_running = running;

    player->jump_buffer_timer -= dt;
    if (player->jump_buffer_timer > 0.0f && player->grounded) {
        player->dy = PLAYER_JUMP_FORCE;
        player->jump_buffer_timer = 0.0f;
    }

    const float friction = (fabsf(target_dx) < PLAYER_EPSILON) ? 0.80f : 0.4f;
    player->dx += (target_dx - player->dx) * friction;
    player->dy += PLAYER_GRAVITY;
    player->dy *= 0.96f;

    if (fabsf(player->dx - target_dx) < PLAYER_EPSILON)
        player->dx = target_dx;

    if (fabsf(player->dy) < PLAYER_EPSILON)
        player->dy = 0.0f;

    // stepping: any movement greater than 33% of grid size (ie. 0.33) will increase the number of `steps` here.
    // these steps will break down the full movement into smaller iterations to avoid jumping over grid collisions.
    const int steps = (int)ceilf((fabsf(player->dx) + fabsf(player->dy)) / 0.33f);
    if (steps > 0) {
        int n = 0;
        while (n < steps) {
            player->xr += player->dx / steps;

            if (player->dx != 0.0f)
                player_on_pre_step_x(player, tilemap);      // add x collision checks and physics in here

            while (player->xr > 1) { player->xr--; player->cx++; }
            while (player->xr < 0) { player->xr++; player->cx--; }

            player->yr += player->dy / steps;

            if (player->dy != 0.0f)
                player_on_pre_step_y(player, tilemap);     // add y collision checks and physics in here

            while (player->yr > 1) { player->yr--; player->cy++; }
            while (player->yr < 0) { player->yr++; player->cy--; }

            ++n;
        }
    }

    player->xx = (player->cx + player->xr) * TILE_RENDER_SIZE;
    player->yy = (player->cy + player->yr) * TILE_RENDER_SIZE;

    player->grounded = is_grounded(player, tilemap);

    if (player->grounded && !player->prev_grounded) {
        player->is_landing = true;
        player->landing_timer = 0.0f;
        player->anim_frame = 7;
        player->anim_timer = 0.0f;
    }
}

void player_update(Player *player, float dt, Input *input, float alpha, Tilemap *tilemap)
{
    if (input_key_pressed(input, SDL_SCANCODE_F3))
        g_render_hitbox = !g_render_hitbox;

    if (input_key_pressed(input, SDL_SCANCODE_SPACE))
        player->jump_buffer_timer = PLAYER_JUMP_BUFFER;

    if (player->is_landing) {
        player->landing_timer += dt;
        const int landing_frame = 7 + (int)(player->landing_timer / (PLAYER_FRAME_TIME * 0.5f));

        if (landing_frame > 9) {
            player->is_landing = false;
            player->anim_frame = 0;
            player->anim_timer = 0.0f;
        } else {
            player->anim_frame = landing_frame;
        }
    } else if (!player->grounded) {
        // ascent
        if (player->dy > 0.0f) {
            float t = 1.0f - (player->dy / PLAYER_JUMP_FORCE);
            t = SDL_clamp(t, 0.0f, 1.0f);
            t = sqrtf(t);
            const int frame = (int)(t * 5.0f);
            player->anim_frame = (frame > 4) ? 4 : frame;
        } else {
            // descent
            player->anim_timer += dt;
            const int loop_frame = (int)(player->anim_timer / PLAYER_FRAME_TIME) % 2;
            player->anim_frame = 4 + loop_frame;
        }
    } else {
        player->anim_timer += dt;
        if (player->anim_timer >= PLAYER_FRAME_TIME) {
            player->anim_timer -= PLAYER_FRAME_TIME;
            player->anim_frame = (player->anim_frame + 1) % anim_cols(player);
        }
    }

    player->render_xx = lerpf(player->prev_xx, player->xx, alpha);
    player->render_yy = lerpf(player->prev_yy, player->yy, alpha);
}

void player_render(Player *player, Shader *shader)
{
    const int cols = anim_cols(player);
    const int rows = anim_rows(player);
    const int frame = player->anim_frame;
    const int dir_row = 0; // animations are single-row sheets

    // size of one frame in uv space
    float scale_x = 1.0f / cols;
    float scale_y = 1.0f / rows;

    // uv origin of current frame
    float offset_x = frame * scale_x;
    float offset_y = dir_row * scale_y;

    // read uv from right to left
    if (player->dir == DIRECTION_W) {
        offset_x += scale_x;
        scale_x = -scale_x;
    }

    shader_set_vec2(shader, "uv_scale", scale_x, scale_y);
    shader_set_vec2(shader, "uv_offset", offset_x, offset_y);

    sprite_render(
        &player->sprite,
        anim_texture(player),
        shader,
        player->render_xx, player->render_yy,
        PLAYER_RENDER_SIZE, PLAYER_RENDER_SIZE,
        0.0f,
        0.5f, 0.0f);

    if (g_render_hitbox)
        player_render_hitbox(player, shader);
}
