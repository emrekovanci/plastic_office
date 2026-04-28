#include "input.h"

void input_update(Input *input)
{
    SDL_memcpy(input->prev, input->curr, sizeof(input->prev));
    const bool *state = SDL_GetKeyboardState(NULL);
    SDL_memcpy(input->curr, state, sizeof(input->curr));

    SDL_GetMouseState(&input->mouse_x, &input->mouse_y);
    input->prev_mouse_right = input->curr_mouse_right;
    input->curr_mouse_right = input_mouse_right_down();
}

bool input_key_down(Input *input, SDL_Scancode key)
{
    return input->curr[key] == true;
}

bool input_key_up(Input *input, SDL_Scancode key)
{
    return input->curr[key] == false;
}

bool input_key_pressed(Input *input, SDL_Scancode key)
{
    return input->curr[key] == true && input->prev[key] == false;
}

bool input_key_released(Input *input, SDL_Scancode key)
{
    return input->curr[key] == false && input->prev[key] == true;
}

bool input_mouse_left_down(void)
{
    SDL_MouseButtonFlags flags = SDL_GetMouseState(NULL, NULL);
    return flags & SDL_BUTTON_MASK(SDL_BUTTON_LEFT);
}

bool input_mouse_middle_down(void)
{
    SDL_MouseButtonFlags flags = SDL_GetMouseState(NULL, NULL);
    return flags & SDL_BUTTON_MASK(SDL_BUTTON_MIDDLE);
}

bool input_mouse_right_down(void)
{
    SDL_MouseButtonFlags flags = SDL_GetMouseState(NULL, NULL);
    return flags & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT);
}

bool input_mouse_right_pressed(Input *input)
{
    return input->curr_mouse_right == true && input->prev_mouse_right == false;
}

bool input_mouse_right_released(Input *input)
{
    return input->curr_mouse_right == false && input->prev_mouse_right == true;
}
