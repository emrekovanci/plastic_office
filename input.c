#include "input.h"

void input_update(Input *input)
{
    SDL_memcpy(input->prev_kb, input->curr_kb, sizeof(input->prev_kb));
    const bool *state = SDL_GetKeyboardState(NULL);
    SDL_memcpy(input->curr_kb, state, sizeof(input->curr_kb));

    input->prev_mouse = input->curr_mouse;
    input->curr_mouse = SDL_GetMouseState(&input->mouse_x, &input->mouse_y);
}

bool input_key_down(Input *input, SDL_Scancode key)
{
    return input->curr_kb[key] == true;
}

bool input_key_up(Input *input, SDL_Scancode key)
{
    return input->curr_kb[key] == false;
}

bool input_key_pressed(Input *input, SDL_Scancode key)
{
    return input->curr_kb[key] == true &&
           input->prev_kb[key] == false;
}

bool input_key_released(Input *input, SDL_Scancode key)
{
    return input->curr_kb[key] == false &&
           input->prev_kb[key] == true;
}

bool input_mouse_down(Input *input, int btn)
{
    return (input->curr_mouse & SDL_BUTTON_MASK(btn)) != 0;
}

bool input_mouse_up(Input *input, int btn)
{
    return (input->curr_mouse & SDL_BUTTON_MASK(btn)) == 0;
}

bool input_mouse_pressed(Input *input, int btn)
{
    return (input->curr_mouse & SDL_BUTTON_MASK(btn)) != 0 &&
           (input->prev_mouse & SDL_BUTTON_MASK(btn)) == 0;
}

bool input_mouse_released(Input *input, int btn)
{
    return (input->curr_mouse & SDL_BUTTON_MASK(btn)) == 0 &&
           (input->prev_mouse & SDL_BUTTON_MASK(btn)) != 0;
}
