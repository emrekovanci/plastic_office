#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include <SDL3/SDL.h>

typedef struct Input Input;
struct Input {
    bool prev_kb[SDL_SCANCODE_COUNT];
    bool curr_kb[SDL_SCANCODE_COUNT];
    float mouse_x;
    float mouse_y;
    SDL_MouseButtonFlags prev_mouse;
    SDL_MouseButtonFlags curr_mouse;
};

void input_update(Input *input);

bool input_key_down(Input *input, SDL_Scancode key);
bool input_key_up(Input *input, SDL_Scancode key);
bool input_key_pressed(Input *input, SDL_Scancode key);
bool input_key_released(Input *input, SDL_Scancode key);

bool input_mouse_down(Input *input, int btn);
bool input_mouse_up(Input *input, int btn);
bool input_mouse_pressed(Input *input, int btn);
bool input_mouse_released(Input *input, int btn);

#endif
