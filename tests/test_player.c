#include <unity.h>

#include <player.h>
#include <constants.h>

#define FIXED_TICKS 30
#define PLAYER_CY   (MAP_ROWS / 2)
#define PLAYER_CX   (MAP_COLS / 2)

static Player make_player(void)
{
    Player p = {0};
    p.cx = PLAYER_CX;
    p.xr = 0.5f;
    p.cy = PLAYER_CY;
    p.yr = 0.0f;

    return p;
}

static Tilemap make_empty_tilemap(void)
{
    Tilemap t = {0};
    memset(t.collision, 0xFF, sizeof(t.collision));

    return t;
}

static Tilemap make_tilemap_with_ground(void)
{
    Tilemap t = {0};
    memset(t.collision, 0xFF, sizeof(t.collision));

    // ground under player
    for (int x = 0; x < MAP_COLS; ++x)
        t.collision[PLAYER_CY - 1][x] = 1;

    return t;
}

static Input make_input_right(void)
{
    Input input = {0};
    input.curr_kb[SDL_SCANCODE_D] = true;
    input.curr_kb[SDL_SCANCODE_RIGHT] = true;

    return input;
}

static Input make_input_left(void)
{
    Input input = {0};
    input.curr_kb[SDL_SCANCODE_A] = true;
    input.curr_kb[SDL_SCANCODE_LEFT] = true;

    return input;
}

void test_player_moves_right_when_right_key_held(void)
{
    Player p = make_player();
    Tilemap t = make_tilemap_with_ground();
    Input input = make_input_right();

    float before_right_x = p.cx + p.xr + PLAYER_HITBOX_HALF_WIDTH;
    for (int i = 0; i < FIXED_TICKS; ++i)
        player_fixed_update(&p, FIXED_TIMESTEP, &input, &t);
    float after_right_x = p.cx + p.xr + PLAYER_HITBOX_HALF_WIDTH;

    TEST_ASSERT_GREATER_THAN_FLOAT(before_right_x, after_right_x);
    TEST_ASSERT_TRUE(p.grounded);
}

void test_player_moves_left_when_left_key_held(void)
{
    Player p = make_player();
    Tilemap t = make_tilemap_with_ground();
    Input input = make_input_left();

    float before_right_x = p.cx + p.xr + PLAYER_HITBOX_HALF_WIDTH;
    for (int i = 0; i < FIXED_TICKS; ++i)
        player_fixed_update(&p, FIXED_TIMESTEP, &input, &t);
    float after_right_x = p.cx + p.xr + PLAYER_HITBOX_HALF_WIDTH;

    TEST_ASSERT_LESS_THAN_FLOAT(before_right_x, after_right_x);
    TEST_ASSERT_TRUE(p.grounded);
}

void test_player_falls_without_ground(void)
{
    Player p = make_player();
    Tilemap t = make_empty_tilemap();
    Input input = {0};

    float before_y = p.cy + p.yr;
    for (int i = 0; i < FIXED_TICKS; ++i)
        player_fixed_update(&p, FIXED_TIMESTEP, &input, &t);
    float after_y = p.cy + p.yr;

    TEST_ASSERT_LESS_THAN_FLOAT(before_y, after_y);
    TEST_ASSERT_FALSE(p.grounded);
}

void test_player_does_not_fall_with_ground(void)
{
    Player p = make_player();
    Tilemap t = make_tilemap_with_ground();
    Input input = {0};

    float before_y = p.cy + p.yr;
    for (int i = 0; i < FIXED_TICKS; ++i)
        player_fixed_update(&p, FIXED_TIMESTEP, &input, &t);
    float after_y = p.cy + p.yr;

    TEST_ASSERT_FLOAT_WITHIN(0.0001f, before_y, after_y);
    TEST_ASSERT_TRUE(p.grounded);
}
