#include <unity.h>

// test_tilemap.c
extern void test_collision_solid_tile(void);
extern void test_collision_empty_tile(void);
extern void test_collision_any_positive_value_is_solid(void);
extern void test_collision_cells_are_independent(void);
extern void test_read_returns_false_for_missing_file(void);
extern void test_read_loads_bottom_to_top(void);
extern void test_read_negative_tile_ids(void);

// test_input.c
extern void test_key_down_when_curr_true(void);
extern void test_key_down_when_curr_false(void);
extern void test_key_up_when_curr_true(void);
extern void test_key_up_when_curr_false(void);
extern void test_key_pressed_when_curr_true_prev_false(void);
extern void test_key_pressed_when_held(void);
extern void test_key_pressed_when_not_held(void);
extern void test_key_released_when_curr_false_prev_true(void);
extern void test_key_released_when_still_held(void);
extern void test_mouse_down_when_button_held(void);
extern void test_mouse_down_when_button_not_held(void);
extern void test_mouse_up_when_button_held(void);
extern void test_mouse_up_when_button_not_held(void);
extern void test_mouse_pressed_when_curr_true_prev_false(void);
extern void test_mouse_pressed_when_held(void);
extern void test_mouse_released_when_curr_false_prev_true(void);
extern void test_mouse_released_when_still_held(void);

void setUp(void) { }
void tearDown(void) { }

int main(void)
{
    UNITY_BEGIN();

    // test_tilemap
    RUN_TEST(test_collision_solid_tile);
    RUN_TEST(test_collision_empty_tile);
    RUN_TEST(test_collision_any_positive_value_is_solid);
    RUN_TEST(test_collision_cells_are_independent);
    RUN_TEST(test_read_returns_false_for_missing_file);
    RUN_TEST(test_read_loads_bottom_to_top);
    RUN_TEST(test_read_negative_tile_ids);

    // test_input
    RUN_TEST(test_key_down_when_curr_true);
    RUN_TEST(test_key_down_when_curr_false);
    RUN_TEST(test_key_up_when_curr_true);
    RUN_TEST(test_key_up_when_curr_false);
    RUN_TEST(test_key_pressed_when_curr_true_prev_false);
    RUN_TEST(test_key_pressed_when_held);
    RUN_TEST(test_key_pressed_when_not_held);
    RUN_TEST(test_key_released_when_curr_false_prev_true);
    RUN_TEST(test_key_released_when_still_held);
    RUN_TEST(test_mouse_down_when_button_held);
    RUN_TEST(test_mouse_down_when_button_not_held);
    RUN_TEST(test_mouse_up_when_button_held);
    RUN_TEST(test_mouse_up_when_button_not_held);
    RUN_TEST(test_mouse_pressed_when_curr_true_prev_false);
    RUN_TEST(test_mouse_pressed_when_held);
    RUN_TEST(test_mouse_released_when_curr_false_prev_true);
    RUN_TEST(test_mouse_released_when_still_held);
    
    return UNITY_END();
}
