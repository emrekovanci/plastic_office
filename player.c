#include "player.h"
#include "constants.h"
#include "utils.h"

// ================
// hitbox debugging
// ================
bool g_render_hitbox = false;
Texture g_texture_hitbox = {0};
static void player_render_hitbox(Player *player, Shader *shader)
{
    const float width = PLAYER_HITBOX_HALF_WIDTH * 2.0f * TILE_RENDER_SIZE;
    const float height = PLAYER_HITBOX_HEIGHT * TILE_RENDER_SIZE;

    shader_set_vec2(shader, "uv_scale",  1.0f, 1.0f);
    shader_set_vec2(shader, "uv_offset", 0.0f, 0.0f);

    sprite_render(
        &player->sprite,
        &g_texture_hitbox,
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
    return 1;
}

static int anim_cols(Player *player)
{
    if (!player->grounded || player->is_landing) return 10;
    if (player->is_moving && player->is_running) return 10;
    if (player->is_moving) return 10;
    return 6;
}

static bool is_grounded(Player *player, Tilemap *tilemap)
{
    if (player->yr > 0.01f)
        return false;

    int x_min = (int)floorf(player->cx + player->xr - PLAYER_HITBOX_HALF_WIDTH + 0.01f);
    int x_max = (int)floorf(player->cx + player->xr + PLAYER_HITBOX_HALF_WIDTH - 0.01f);

    return collides_y(tilemap, player->cy - 1, x_min, x_max);
}

void player_init(Player *player)
{
    sprite_init(&player->sprite);
    texture_load("assets/Gangsters_1/Idle.png", &player->texture_idle);
    texture_load("assets/Gangsters_1/Walk.png", &player->texture_walk);
    texture_load("assets/Gangsters_1/Run.png",  &player->texture_run);
    texture_load("assets/Gangsters_1/Jump.png",  &player->texture_jump);
    texture_create_solid(&g_texture_hitbox, 255, 0, 0, 100);

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
    player->dir = DIRECTION_E;
    player->is_moving = false;
    player->is_running = false;
    player->jump_pressed = false;
    player->grounded = false;
    player->prev_grounded = false;
    player->is_landing = false;
    player->landing_timer = 0.0f;
}

void player_destroy(Player *player)
{
    texture_destroy(&g_texture_hitbox);
    texture_destroy(&player->texture_jump);
    texture_destroy(&player->texture_run);
    texture_destroy(&player->texture_walk);
    texture_destroy(&player->texture_idle);
    sprite_destroy(&player->sprite);
}

void player_on_pre_step_x(Player *player, Tilemap *tilemap)
{
    const int y_min = (int)floorf(player->cy + player->yr);
    const int y_max = (int)floorf(player->cy + player->yr + PLAYER_HITBOX_HEIGHT - 0.01f);

    // right collision
    if (player->xr >= (1.0f - PLAYER_HITBOX_HALF_WIDTH)) {
        if (collides_x(tilemap, player->cx + 1, y_min, y_max)) {
            player->xr = 1.0f - PLAYER_HITBOX_HALF_WIDTH;
            player->dx = 0.0f;
        }
    }

    // left collision
    if (player->xr <= PLAYER_HITBOX_HALF_WIDTH) {
        if (collides_x(tilemap, player->cx - 1, y_min, y_max)) {
            player->xr = PLAYER_HITBOX_HALF_WIDTH;
            player->dx = 0.0f;
        }
    }
}

void player_on_pre_step_y(Player *player, Tilemap *tilemap)
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
    float top_y = player->cy + player->yr + PLAYER_HITBOX_HEIGHT;
    int top_cy = (int)floorf(top_y);

    if (top_y - top_cy > 0.0f) {
        if (collides_y(tilemap, top_cy, x_min, x_max)) {
            player->yr = top_cy - player->cy - PLAYER_HITBOX_HEIGHT;
            if (player->dy > 0.0f)
                player->dy = 0.0f;
        }
    }
}

void player_fixed_update(Player *player, float dt, Input *input, Tilemap *tilemap)
{
    // save prev physic position for interpolation
    player->prev_xx = player->xx;
    player->prev_yy = player->yy;

    // save prev grounded state for landing transition
    player->prev_grounded = player->grounded;

    player->is_moving =
        input_key_down(input, SDL_SCANCODE_A) || input_key_down(input, SDL_SCANCODE_LEFT) ||
        input_key_down(input, SDL_SCANCODE_D) || input_key_down(input, SDL_SCANCODE_RIGHT);

    player->is_running =
        (player->is_moving) &&
        (input_key_down(input, SDL_SCANCODE_LSHIFT) || input_key_down(input, SDL_SCANCODE_RSHIFT));

    const float speed = (player->is_running) ? PLAYER_RUN_SPEED : PLAYER_MOVE_SPEED;

    float target_dx = 0.0f;

    if (input_key_down(input, SDL_SCANCODE_A) || input_key_down(input, SDL_SCANCODE_LEFT)) {
        player->dir = DIRECTION_W;
        target_dx = -speed;
    }

    if (input_key_down(input, SDL_SCANCODE_D) || input_key_down(input, SDL_SCANCODE_RIGHT)) {
        player->dir = DIRECTION_E;
        target_dx = speed;
    }

    if (player->jump_pressed) {
        player->dy = PLAYER_JUMP_FORCE;
        player->jump_pressed = false;
    }

    player->dx += (target_dx - player->dx) * 0.4f;
    player->dy += PLAYER_GRAVITY;
    player->dy *= 0.96f;

    if (fabsf(player->dx) < 0.001f) player->dx = 0.0f;
    if (fabsf(player->dy) < 0.001f) player->dy = 0.0f;

    // stepping: any movement greater than 33% of grid size (ie. 0.33) will increase the number of `steps` here.
    // these steps will break down the full movement into smaller iterations to avoid jumping over grid collisions.
    int steps = (int)ceilf((fabsf(player->dx) + fabsf(player->dy)) / 0.33f);
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

    if (!player->is_moving)
        player->is_moving = fabsf(player->dx) > 0.01f;

    player->grounded = is_grounded(player, tilemap);
}

void player_update(Player *player, float dt, Input *input, float alpha, Tilemap *tilemap)
{
    if (input_key_pressed(input, SDL_SCANCODE_F4))
        g_render_hitbox = !g_render_hitbox;

    if (input_key_pressed(input, SDL_SCANCODE_SPACE) && player->grounded)
        player->jump_pressed = true;

    if (player->grounded && !player->prev_grounded) {
        player->is_landing = true;
        player->landing_timer = 0.0f;
        player->anim_frame = 7;
        player->anim_timer = 0.0f;
    }

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
            t = SDL_sqrtf(t);
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

void player_set_coords(Player *player, float x, float y)
{
    player->xx = x;
    player->yy = y;
    player->cx = (int)(player->xx / TILE_RENDER_SIZE);
    player->cy = (int)(player->yy / TILE_RENDER_SIZE);
    player->xr = (player->xx - player->cx * TILE_RENDER_SIZE) / TILE_RENDER_SIZE;
    player->yr = (player->yy - player->cy * TILE_RENDER_SIZE) / TILE_RENDER_SIZE;
}
