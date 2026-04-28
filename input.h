#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include <SDL3/SDL.h>

typedef struct Input Input;
struct Input {
    bool prev[SDL_SCANCODE_COUNT];
    bool curr[SDL_SCANCODE_COUNT];
    float mouse_x;
    float mouse_y;
    bool prev_mouse_right;
    bool curr_mouse_right;
};

void input_update(Input *input);

bool input_key_down(Input *input, SDL_Scancode key);
bool input_key_up(Input *input, SDL_Scancode key);
bool input_key_pressed(Input *input, SDL_Scancode key);
bool input_key_released(Input *input, SDL_Scancode key);

bool input_mouse_left_down(void);
bool input_mouse_middle_down(void);
bool input_mouse_right_down(void);

bool input_mouse_right_pressed(Input *input);
bool input_mouse_right_released(Input *input);

#endif
