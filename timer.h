#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>
#include <SDL3/SDL_stdinc.h>

typedef struct Timer {
    float timescale;
    float fixed_timestep;
    Uint64 last_tick;
    double accumulator;
} Timer;

void timer_init(Timer *t, float fixed_timestep);
float timer_tick(Timer *t);
bool timer_step(Timer *t);
float timer_alpha(const Timer *t);

#endif
