#include "timer.h"

#include <SDL3/SDL_timer.h>

void timer_init(Timer *t, float fixed_timestep)
{
    t->timescale = 1.0f;
    t->fixed_timestep = fixed_timestep;
    t->last_tick = SDL_GetTicksNS();
    t->accumulator = 0.0;
}

float timer_tick(Timer *t)
{
    Uint64 now = SDL_GetTicksNS();
    float dt = (float)(now - t->last_tick) / (double)SDL_NS_PER_SECOND;
    dt = SDL_min(dt, 0.25f);
    dt *= t->timescale;
    t->last_tick = now;
    t->accumulator += dt;
    return dt;
}

bool timer_step(Timer *t)
{
    if (t->accumulator < t->fixed_timestep)
        return false;

    t->accumulator -= t->fixed_timestep;
    return true;
}

float timer_alpha(const Timer *t)
{
    return (float)(t->accumulator / t->fixed_timestep);
}
