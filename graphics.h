#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include "defs.h"

using namespace std;

struct Graphics
{
    // Initialize
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;
    TTF_Font* titleFont = nullptr;

    SDL_Texture* backgroundTexture = nullptr;
    SDL_Texture* basketTexture = nullptr;
    SDL_Texture* itemTexture = nullptr;
    SDL_Texture* heartTexture = nullptr;
    SDL_Texture* playButtonTexture = nullptr;
    SDL_Texture* pauseButtonTexture = nullptr;
    SDL_Texture* yesButtonTexture = nullptr;
    SDL_Texture* noButtonTexture = nullptr;
    SDL_Texture* gameOverTexture = nullptr;
    SDL_Texture* restartButtonTexture = nullptr;
    SDL_Texture* homeButtonTexture = nullptr;
    SDL_Texture* titleTextTexture = nullptr;
    SDL_Texture* scoreTextTexture = nullptr;
    SDL_Texture* pauseTextTexture = nullptr;

    // Functions
    void logErrorAndExit(const char* msg, const char* error);
    void init();
    void destroy();
    SDL_Texture* loadTexture(const char* filename);
    void loadGameTextures();
    SDL_Texture* createTextTexture(const string& text, SDL_Color color, TTF_Font* targetFont);
    void updateScoreTexture(int score);
    void createPauseTextTexture();
    void clearScreen();
    void presentScene();
    void renderTexture(SDL_Texture* texture, const SDL_Rect* src, const SDL_Rect* dest);
    void renderTexture(SDL_Texture* texture, int x, int y);
    void renderFilledRect(const SDL_Rect* rect, SDL_Color color);
    void setDrawBlendMode(SDL_BlendMode mode);
};

#endif // GRAPHICS_H
