#include <unity.h>

#include <SDL3/SDL.h>
#include <tilemap.h>

void test_collision_solid_tile(void)
{
    Tilemap t = {0};
    t.collision[0][0] = 1;
    TEST_ASSERT_TRUE(tilemap_has_collision(&t, 0, 0));
}

void test_collision_empty_tile(void)
{
    Tilemap t = {0};
    t.collision[0][0] = -1;
    TEST_ASSERT_FALSE(tilemap_has_collision(&t, 0, 0));
}

void test_collision_any_positive_value_is_solid(void)
{
    Tilemap t = {0};
    t.collision[0][0] = 99;
    TEST_ASSERT_TRUE(tilemap_has_collision(&t, 0, 0));
}

void test_collision_cells_are_independent(void)
{
    Tilemap t = {0};
    t.collision[0][0] = 1;
    t.collision[0][1] = -1;
    TEST_ASSERT_TRUE(tilemap_has_collision(&t, 0, 0));
    TEST_ASSERT_FALSE(tilemap_has_collision(&t, 1, 0));
}

void test_collision_out_of_bounds_negative(void)
{
    Tilemap t = {0};
    TEST_ASSERT_FALSE(tilemap_has_collision(&t, -1, 0));
    TEST_ASSERT_FALSE(tilemap_has_collision(&t, 0, -1));
}

void test_collision_out_of_bounds_positive(void)
{
    Tilemap t = {0};
    TEST_ASSERT_FALSE(tilemap_has_collision(&t, MAP_COLS, 0));
    TEST_ASSERT_FALSE(tilemap_has_collision(&t, 0, MAP_ROWS));
}

void test_collision_boundary_coords_are_valid(void)
{
    Tilemap t = {0};
    t.collision[0][0] = 1;
    t.collision[MAP_ROWS - 1][MAP_COLS - 1] = 1;
    TEST_ASSERT_TRUE(tilemap_has_collision(&t, 0, 0));
    TEST_ASSERT_TRUE(tilemap_has_collision(&t, MAP_COLS - 1, MAP_ROWS - 1));
}

void test_read_returns_false_for_missing_file(void)
{
    int map[MAP_ROWS][MAP_COLS] = {0};
    bool result = tilemap_read("nonexistent.csv", map);
    TEST_ASSERT_FALSE(result);
}

void test_read_loads_bottom_to_top(void)
{
    int map[MAP_ROWS][MAP_COLS] = {0};
    tilemap_read("test_assets/simple.csv", map);
    TEST_ASSERT_EQUAL_INT(1, map[MAP_ROWS-1][0]);
    TEST_ASSERT_EQUAL_INT(3, map[MAP_ROWS-2][0]);
}

void test_read_negative_tile_ids(void)
{
    int map[MAP_ROWS][MAP_COLS] = {0};
    tilemap_read("test_assets/negative.csv", map);
    TEST_ASSERT_EQUAL_INT(-1, map[MAP_ROWS-1][0]);
    TEST_ASSERT_EQUAL_INT(-4, map[MAP_ROWS-2][1]);
}
