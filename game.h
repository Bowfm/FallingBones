#ifndef GAME_H
#define GAME_H

#include <vector>
#include <SDL.h>
#include "graphics.h"
#include "audio.h"
#include "structs.h"
#include "defs.h"

using namespace std;

struct Game
{
    // Initial game state
    GameState currentState = GameState::MENU;
    int score = 0;
    int lives = 3;
    bool gamePausedConfirm = false;
    bool isLeftKeyDown = false;
    bool isRightKeyDown = false;

    // Game objects
    SDL_Rect basketRect;
    int basketVelX = 0;
    vector<Item> items;

    // Buttons
    SDL_Rect playButtonRect;
    SDL_Rect pauseButtonRect;
    SDL_Rect yesButtonRect;
    SDL_Rect noButtonRect;
    SDL_Rect restartButtonRect;
    SDL_Rect homeButtonRect;

    // Functions
    void init();
    void reset(Audio& audio);
    void handleInput(SDL_Event& e, bool& quit, Graphics& graphics, Audio& audio);
    void update(Graphics& graphics, Audio& audio);
    void render(Graphics& graphics);
    void stopBasketMovement();
};

#endif // GAME_H
