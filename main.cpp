#include <SDL.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "defs.h"
#include "structs.h"
#include "graphics.h"
#include "audio.h"
#include "game.h"

int main(int argc, char* argv[])
{
    srand(time(0));

    Graphics graphics;
    Audio audio;
    Game game;

    graphics.init();
    audio.init();
    graphics.loadGameTextures();
    audio.loadSounds();
    graphics.updateScoreTexture(game.score);
    game.init();

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
             if (e.type == SDL_QUIT) {
                 quit = true;
             }
             game.handleInput(e, quit, graphics, audio);
        }
        game.update(graphics, audio);
        graphics.clearScreen();
        game.render(graphics);
        graphics.presentScene();
        SDL_Delay(16);
    }
    graphics.destroy();
    audio.destroy();
    SDL_Quit();

    return 0;
}
