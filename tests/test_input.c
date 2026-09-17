#include <unity.h>

#include <SDL3/SDL.h>
#include <input.h>

void test_key_down_when_curr_true(void)
{
    Input input = {0};
    input.curr_kb[SDL_SCANCODE_A] = true;

    TEST_ASSERT_TRUE(input_key_down(&input, SDL_SCANCODE_A));
}

void test_key_down_when_curr_false(void)
{
    Input input = {0};

    TEST_ASSERT_FALSE(input_key_down(&input, SDL_SCANCODE_A));
}

void test_key_up_when_curr_true(void)
{
    Input input = {0};
    input.curr_kb[SDL_SCANCODE_A] = true;

    TEST_ASSERT_FALSE(input_key_up(&input, SDL_SCANCODE_A));
}

void test_key_up_when_curr_false(void)
{
    Input input = {0};

    TEST_ASSERT_TRUE(input_key_up(&input, SDL_SCANCODE_A));
}

void test_key_pressed_when_curr_true_prev_false(void)
{
    Input input = {0};
    input.curr_kb[SDL_SCANCODE_A] = true;
    input.prev_kb[SDL_SCANCODE_A] = false;

    TEST_ASSERT_TRUE(input_key_pressed(&input, SDL_SCANCODE_A));
}

void test_key_pressed_when_held(void)
{
    Input input = {0};
    input.curr_kb[SDL_SCANCODE_A] = true;
    input.prev_kb[SDL_SCANCODE_A] = true;

    TEST_ASSERT_FALSE(input_key_pressed(&input, SDL_SCANCODE_A));
}

void test_key_pressed_when_not_held(void)
{
    Input input = {0};

    TEST_ASSERT_FALSE(input_key_pressed(&input, SDL_SCANCODE_A));
}

void test_key_released_when_curr_false_prev_true(void)
{
    Input input = {0};
    input.curr_kb[SDL_SCANCODE_A] = false;
    input.prev_kb[SDL_SCANCODE_A] = true;

    TEST_ASSERT_TRUE(input_key_released(&input, SDL_SCANCODE_A));
}

void test_key_released_when_still_held(void)
{
    Input input = {0};
    input.curr_kb[SDL_SCANCODE_A] = true;
    input.prev_kb[SDL_SCANCODE_A] = true;

    TEST_ASSERT_FALSE(input_key_released(&input, SDL_SCANCODE_A));
}

void test_mouse_down_when_button_held(void)
{
    Input input = {0};
    input.curr_mouse = SDL_BUTTON_MASK(SDL_BUTTON_LEFT);

    TEST_ASSERT_TRUE(input_mouse_down(&input, SDL_BUTTON_LEFT));
}

void test_mouse_down_when_button_not_held(void)
{
    Input input = {0};

    TEST_ASSERT_FALSE(input_mouse_down(&input, SDL_BUTTON_LEFT));
}

void test_mouse_up_when_button_held(void)
{
    Input input = {0};
    input.curr_mouse = SDL_BUTTON_MASK(SDL_BUTTON_LEFT);

    TEST_ASSERT_FALSE(input_mouse_up(&input, SDL_BUTTON_LEFT));
}

void test_mouse_up_when_button_not_held(void)
{
    Input input = {0};

    TEST_ASSERT_TRUE(input_mouse_up(&input, SDL_BUTTON_LEFT));
}

void test_mouse_pressed_when_curr_true_prev_false(void)
{
    Input input = {0};
    input.curr_mouse = SDL_BUTTON_MASK(SDL_BUTTON_LEFT);
    input.prev_mouse = 0;

    TEST_ASSERT_TRUE(input_mouse_pressed(&input, SDL_BUTTON_LEFT));
}

void test_mouse_pressed_when_held(void)
{
    Input input = {0};
    input.curr_mouse = SDL_BUTTON_MASK(SDL_BUTTON_LEFT);
    input.prev_mouse = SDL_BUTTON_MASK(SDL_BUTTON_LEFT);

    TEST_ASSERT_FALSE(input_mouse_pressed(&input, SDL_BUTTON_LEFT));
}

void test_mouse_released_when_curr_false_prev_true(void)
{
    Input input = {0};
    input.curr_mouse = 0;
    input.prev_mouse = SDL_BUTTON_MASK(SDL_BUTTON_LEFT);

    TEST_ASSERT_TRUE(input_mouse_released(&input, SDL_BUTTON_LEFT));
}

void test_mouse_released_when_still_held(void)
{
    Input input = {0};
    input.curr_mouse = SDL_BUTTON_MASK(SDL_BUTTON_LEFT);
    input.prev_mouse = SDL_BUTTON_MASK(SDL_BUTTON_LEFT);

    TEST_ASSERT_FALSE(input_mouse_released(&input, SDL_BUTTON_LEFT));
}
