#ifndef STRUCTS_H
#define STRUCTS_H

#include <SDL_rect.h>

enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAME_OVER
};

struct Item {
    SDL_Rect rect;
    bool active;
};

#endif // STRUCTS_H
