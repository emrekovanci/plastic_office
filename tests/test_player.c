#include <unity.h>

#include <player.h>
#include <constants.h>

#define PLAYER_CY (MAP_ROWS / 2)
#define PLAYER_CX (MAP_COLS / 2)

static void simulate(Player *player, Input *input, Tilemap *tilemap, int steps)
{
    for (int i = 0; i < steps; ++i)
        player_fixed_update(player, FIXED_TIMESTEP, input, tilemap);
}

static Player make_player(void)
{
    Player p = {0};
    p.cx = PLAYER_CX;
    p.xr = 0.5f;
    p.cy = PLAYER_CY;
    p.yr = 0.0f;

    return p;
}

static void clear_tilemap_collision(Tilemap *t)
{
    for (int y = 0; y < MAP_ROWS; ++y)
        for (int x = 0; x < MAP_COLS; ++x)
            t->layers[COLLISION_LAYER_INDEX].tiles[TILE_INDEX(x, y)] = -1;
}

static Tilemap make_empty_tilemap(void)
{
    Tilemap t = {0};
    clear_tilemap_collision(&t);

    return t;
}

static Tilemap make_tilemap_with_ground(void)
{
    Tilemap t = {0};
    clear_tilemap_collision(&t);

    // ground under player
    for (int x = 0; x < MAP_COLS; ++x)
        t.layers[COLLISION_LAYER_INDEX].tiles[TILE_INDEX(x, PLAYER_CY - 1)] = 1;

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

    float before_x = p.cx + p.xr;
    simulate(&p, &input, &t, 30);
    float after_x = p.cx + p.xr;

    TEST_ASSERT_GREATER_THAN_FLOAT(before_x, after_x);
}

void test_player_faces_right_when_moving_right(void)
{
    Player p = make_player();
    Tilemap t = make_tilemap_with_ground();
    Input input = make_input_right();

    simulate(&p, &input, &t, 1);

    TEST_ASSERT_EQUAL(DIRECTION_E, p.dir);
}

void test_player_moves_left_when_left_key_held(void)
{
    Player p = make_player();
    Tilemap t = make_tilemap_with_ground();
    Input input = make_input_left();

    float before_x = p.cx + p.xr;
    simulate(&p, &input, &t, 30);
    float after_x = p.cx + p.xr;

    TEST_ASSERT_LESS_THAN_FLOAT(before_x, after_x);
}

void test_player_faces_left_when_moving_left(void)
{
    Player p = make_player();
    Tilemap t = make_tilemap_with_ground();
    Input input = make_input_left();

    simulate(&p, &input, &t, 1);

    TEST_ASSERT_EQUAL(DIRECTION_W, p.dir);
}

void test_player_falls_without_ground(void)
{
    Player p = make_player();
    Tilemap t = make_empty_tilemap();
    Input input = {0};

    float before_y = p.cy + p.yr;
    simulate(&p, &input, &t, 30);
    float after_y = p.cy + p.yr;

    TEST_ASSERT_LESS_THAN_FLOAT(before_y, after_y);
}

void test_player_does_not_fall_with_ground(void)
{
    Player p = make_player();
    Tilemap t = make_tilemap_with_ground();
    Input input = {0};

    float before_y = p.cy + p.yr;
    simulate(&p, &input, &t, 30);
    float after_y = p.cy + p.yr;

    TEST_ASSERT_FLOAT_WITHIN(0.0001f, before_y, after_y);
}

void test_player_jump_applies_upward_velocity(void)
{
    Player p = make_player();
    p.grounded = true;
    p.jump_requested = true;
    Tilemap t = make_tilemap_with_ground();
    Input input = {0};

    float before_y = p.cy + p.yr;
    simulate(&p, &input, &t, 3);
    float after_y = p.cy + p.yr;

    TEST_ASSERT_GREATER_THAN_FLOAT(before_y, after_y);
}

void test_player_is_not_grounded_after_jump(void)
{
    Player p = make_player();
    p.grounded = true;
    p.jump_requested = true;
    Tilemap t = make_tilemap_with_ground();
    Input input = {0};

    simulate(&p, &input, &t, 3);

    TEST_ASSERT_FALSE(p.grounded);
}
