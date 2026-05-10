#include <unity.h>

// test_tilemap.c
extern void test_collision_solid_tile(void);
extern void test_collision_empty_tile(void);
extern void test_collision_any_zero_value_is_solid(void);
extern void test_collision_any_positive_value_is_solid(void);
extern void test_collision_cells_are_independent(void);
extern void test_collision_out_of_bounds_negative(void);
extern void test_collision_out_of_bounds_positive(void);
extern void test_collision_boundary_coords_are_valid(void);
extern void test_read_returns_false_for_missing_file(void);
extern void test_read_loads_bottom_to_top(void);
extern void test_read_negative_tile_ids(void);
extern void test_read_returns_false_for_too_few_cols(void);
extern void test_read_returns_false_for_too_many_cols(void);

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

// test_player.c
extern void test_player_moves_right_when_right_key_held(void);
extern void test_player_faces_right_when_moving_right(void);
extern void test_player_moves_left_when_left_key_held(void);
extern void test_player_faces_left_when_moving_left(void);
extern void test_player_falls_without_ground(void);
extern void test_player_does_not_fall_with_ground(void);
extern void test_player_jump_applies_upward_velocity(void);
extern void test_player_does_not_jump_when_buffer_empty(void);

void setUp(void) { }
void tearDown(void) { }

int main(void)
{
    UNITY_BEGIN();

    // test_tilemap
    RUN_TEST(test_collision_solid_tile);
    RUN_TEST(test_collision_empty_tile);
    RUN_TEST(test_collision_any_zero_value_is_solid);
    RUN_TEST(test_collision_any_positive_value_is_solid);
    RUN_TEST(test_collision_cells_are_independent);
    RUN_TEST(test_collision_out_of_bounds_negative);
    RUN_TEST(test_collision_out_of_bounds_positive);
    RUN_TEST(test_collision_boundary_coords_are_valid);
    RUN_TEST(test_read_returns_false_for_missing_file);
    RUN_TEST(test_read_loads_bottom_to_top);
    RUN_TEST(test_read_negative_tile_ids);
    RUN_TEST(test_read_returns_false_for_too_few_cols);
    RUN_TEST(test_read_returns_false_for_too_many_cols);

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

    // test_player
    RUN_TEST(test_player_moves_right_when_right_key_held);
    RUN_TEST(test_player_faces_right_when_moving_right);
    RUN_TEST(test_player_moves_left_when_left_key_held);
    RUN_TEST(test_player_faces_left_when_moving_left);
    RUN_TEST(test_player_falls_without_ground);
    RUN_TEST(test_player_does_not_fall_with_ground);
    RUN_TEST(test_player_jump_applies_upward_velocity);
    RUN_TEST(test_player_does_not_jump_when_buffer_empty);

    return UNITY_END();
}
