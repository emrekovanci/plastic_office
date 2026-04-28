#include "direction.h"

#include <SDL3/SDL_stdinc.h>

static int sign(int val)
{
    const int a = 0 < val ? 1 : 0;
    const int b = val < 0 ? 1 : 0;

    return a - b;
}

const int DIR_DELTA_X[8] = {
    [DIRECTION_N]  = 0,
    [DIRECTION_NE] = 1,
    [DIRECTION_E]  = 1,
    [DIRECTION_SE] = 1,
    [DIRECTION_S]  = 0,
    [DIRECTION_SW] = -1,
    [DIRECTION_W]  = -1,
    [DIRECTION_NW] = -1
};

const int DIR_DELTA_Y[8] = {
    [DIRECTION_N]  = -1,
    [DIRECTION_NE] = -1,
    [DIRECTION_E]  = 0,
    [DIRECTION_SE] = 1,
    [DIRECTION_S]  = 1,
    [DIRECTION_SW] = 1,
    [DIRECTION_W]  = 0,
    [DIRECTION_NW] = -1
};

// https://github.com/ClassicUO/ClassicUO/blob/ca0a7113dfdaba6f2ed277ec3f7661625e5f46fe/src/ClassicUO.Client/Game/GameCursor.cs#L670
int get_mouse_direction(int x1, int y1, int to_x, int to_y, int current_facing)
{
    int shiftX = to_x - x1;
    int shiftY = to_y - y1;
    int hashf = 100 * (sign(shiftX) + 2) + 10 * (sign(shiftY) + 2);

    if (shiftX != 0 && shiftY != 0) {
        shiftX = SDL_abs(shiftX);
        shiftY = SDL_abs(shiftY);

        if (shiftY * 5 <= shiftX * 2)
            hashf = hashf + 1;
        else if (shiftY * 2 >= shiftX * 5)
            hashf = hashf + 3;
        else
            hashf = hashf + 2;
    } else if (shiftX == 0) {
        if (shiftY == 0)
            return current_facing;
    }

    switch (hashf) {
        case 111: return DIRECTION_W;
        case 112: return DIRECTION_NW;
        case 113: return DIRECTION_N;
        case 120: return DIRECTION_W;
        case 131: return DIRECTION_W;
        case 132: return DIRECTION_SW;
        case 133: return DIRECTION_S;
        case 210: return DIRECTION_N;
        case 230: return DIRECTION_S;
        case 311: return DIRECTION_E;
        case 312: return DIRECTION_NE;
        case 313: return DIRECTION_N;
        case 320: return DIRECTION_E;
        case 331: return DIRECTION_E;
        case 332: return DIRECTION_SE;
        case 333: return DIRECTION_S;
    }

    return current_facing;
}
