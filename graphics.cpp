#include "graphics.h"
#include <iostream>

void Graphics::logErrorAndExit(const char* msg, const char* error)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "%s: %s", msg, error);
    SDL_Quit();
}

void Graphics::init()
{
    // Init SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        logErrorAndExit("SDL_Init", SDL_GetError());
    }
    // Init SDL_image
    if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG)) {
        logErrorAndExit("SDL_Image", IMG_GetError());
    }

    // Init SDL_ttf
    if (TTF_Init() == -1) {
        logErrorAndExit("SDL_ttf", TTF_GetError());
    }

    // Create window
    window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        logErrorAndExit("Window could not be created! SDL Error", SDL_GetError());
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        logErrorAndExit("Renderer could not be created! SDL Error", SDL_GetError());
    }

     // Open fonts
    font = TTF_OpenFont(FONT, 28);
    if (font == nullptr) {
        logErrorAndExit("Failed to load primary font! TTF_Error", TTF_GetError());
    }

    titleFont = TTF_OpenFont(TITLE_FONT, 80);
     if (titleFont == nullptr) {
        logErrorAndExit("Failed to load title font! TTF_Error", TTF_GetError());
    }

    cout << "Graphics initialized." << endl;
}

void Graphics::destroy()
{
    // Free textures
    SDL_DestroyTexture(backgroundTexture); backgroundTexture = nullptr;
    SDL_DestroyTexture(basketTexture); basketTexture = nullptr;
    SDL_DestroyTexture(itemTexture); itemTexture = nullptr;
    SDL_DestroyTexture(heartTexture); heartTexture = nullptr;
    SDL_DestroyTexture(playButtonTexture); playButtonTexture = nullptr;
    SDL_DestroyTexture(pauseButtonTexture); pauseButtonTexture = nullptr;
    SDL_DestroyTexture(yesButtonTexture); yesButtonTexture = nullptr;
    SDL_DestroyTexture(noButtonTexture); noButtonTexture = nullptr;
    SDL_DestroyTexture(gameOverTexture); gameOverTexture = nullptr;
    SDL_DestroyTexture(restartButtonTexture); restartButtonTexture = nullptr;
    SDL_DestroyTexture(homeButtonTexture); homeButtonTexture = nullptr;
    SDL_DestroyTexture(titleTextTexture); titleTextTexture = nullptr;
    SDL_DestroyTexture(scoreTextTexture); scoreTextTexture = nullptr;
    SDL_DestroyTexture(pauseTextTexture); pauseTextTexture = nullptr;

    // Free fonts
    TTF_CloseFont(font); font = nullptr;
    TTF_CloseFont(titleFont); titleFont = nullptr;
    SDL_DestroyRenderer(renderer); renderer = nullptr;
    SDL_DestroyWindow(window); window = nullptr;

    TTF_Quit();
    IMG_Quit();

    cout << "Graphics destroyed." << endl;
}

SDL_Texture* Graphics::loadTexture(const char* filename)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);
    SDL_Texture *texture = IMG_LoadTexture(renderer, filename);
    if (texture == nullptr) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Load texture %s", IMG_GetError());
    }
    return texture;
}

void Graphics::loadGameTextures()
{
    backgroundTexture = loadTexture(BACKGROUND_IMG);
    basketTexture = loadTexture(BASKET_IMG);
    itemTexture = loadTexture(ITEM_IMG);
    heartTexture = loadTexture(HEART_IMG);
    playButtonTexture = loadTexture(PLAY_IMG);
    pauseButtonTexture = loadTexture(PAUSE_IMG);
    yesButtonTexture = loadTexture(YES_IMG);
    noButtonTexture = loadTexture(NO_IMG);
    gameOverTexture = loadTexture(GAMEOVER_IMG);
    restartButtonTexture = loadTexture(RESTART_IMG);
    homeButtonTexture = loadTexture(HOME_IMG);

    // Load title
    titleTextTexture = createTextTexture(TITLE, {0, 0, 0, 255}, titleFont);

    // Check if all textures loaded successfully
    if (!backgroundTexture || !basketTexture || !itemTexture || !heartTexture || !playButtonTexture || !pauseButtonTexture || !yesButtonTexture || !noButtonTexture || !gameOverTexture || !restartButtonTexture || !homeButtonTexture || !titleTextTexture ) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Failed to load one or more game textures!");
    }
    cout << "All game textures loaded successfully.\n" << endl;
}

SDL_Texture* Graphics::createTextTexture(const string& text, SDL_Color color, TTF_Font* targetFont)
{
    SDL_Surface* textSurface = TTF_RenderText_Solid(targetFont, text.c_str(), color);
    if (textSurface == nullptr) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Render text surface %s", TTF_GetError());
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (texture == nullptr) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Create texture from text %s", SDL_GetError());
    }
    SDL_FreeSurface(textSurface);

    return texture;
}

void Graphics::updateScoreTexture(int score)
{
    // Free score texture (if exists)
    if (scoreTextTexture != nullptr) {
        SDL_DestroyTexture(scoreTextTexture);
        scoreTextTexture = nullptr;
    }

    string scoreStr = "Score: " + to_string(score);
    scoreTextTexture = createTextTexture(scoreStr, {0, 0, 0, 255}, font);

    if (scoreTextTexture == nullptr) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Failed to update score texture!");
    }
}

void Graphics::createPauseTextTexture()
{
    if (pauseTextTexture == nullptr) {
        pauseTextTexture = createTextTexture("Continue?", {255, 255, 255, 255}, font);
        if (pauseTextTexture == nullptr) {
             SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Failed to create pause text texture!");
        }
    }
}

void Graphics::clearScreen()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Graphics::presentScene()
{
    SDL_RenderPresent(renderer);
}

void Graphics::renderTexture(SDL_Texture* texture, const SDL_Rect* src, const SDL_Rect* dest)
{
    SDL_RenderCopy(renderer, texture, src, dest);
}

void Graphics::renderTexture(SDL_Texture* texture, int x, int y)
{
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    SDL_QueryTexture(texture, nullptr, nullptr, &dest.w, &dest.h);
    SDL_RenderCopy(renderer, texture, nullptr, &dest);
}

void Graphics::renderFilledRect(const SDL_Rect* rect, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, rect);
}

void Graphics::setDrawBlendMode(SDL_BlendMode mode)
{
    SDL_SetRenderDrawBlendMode(renderer, mode);
}
