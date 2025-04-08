#include "audio.h"
#include <SDL.h>
#include <iostream>

void Audio::logErrorAndExit(const char* msg, const char* error)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "%s: %s", msg, error);
    SDL_Quit();
}

void Audio::init()
{
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        logErrorAndExit("SDL_mixer could not initialize! Mix_Error", Mix_GetError());
    }
    cout << "Audio initialized." << endl;
}

void Audio::loadSounds()
{
    // Load background music
    bgMusic = Mix_LoadMUS(BGM);
    if (bgMusic == nullptr) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Failed to load background music! Mix_Error: %s", Mix_GetError());
    } else {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading BGM: %s", BGM);
    }

    // Load sound effects
    catchSound = Mix_LoadWAV(CATCH_SFX);
    if (catchSound == nullptr) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Failed to load catch sound effect! Mix_Error: %s", Mix_GetError());
    } else {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading SFX: %s", CATCH_SFX);
    }

    gameOverSound = Mix_LoadWAV(GAMEOVER_SFX);
    if (gameOverSound == nullptr) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Failed to load game over ! Mix_Error: %s", Mix_GetError());
    } else {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading SFX: %s", GAMEOVER_SFX);
    }
    cout << "All audio files loaded successfully.\n" << endl;
}

void Audio::destroy()
{
    Mix_FreeChunk(catchSound); catchSound = nullptr;
    Mix_FreeChunk(gameOverSound); gameOverSound = nullptr;
    Mix_FreeMusic(bgMusic); bgMusic = nullptr;
    Mix_Quit();
    cout << "Audio destroyed." << endl;
}

void Audio::play(Mix_Music* music) {
    if (music != nullptr) {
        Mix_PlayMusic(music, -1);
    }
}

void Audio::play(Mix_Chunk* chunk) {
    if (chunk != nullptr) {
        Mix_PlayChannel(-1, chunk, 0);
    }
}

void Audio::pauseMusic()
{
    if (Mix_PlayingMusic()) {
        Mix_PauseMusic();
    }
}

void Audio::resumeMusic()
{
    if (Mix_PausedMusic()) {
        Mix_ResumeMusic();
    }
}

void Audio::haltMusic()
{
    Mix_HaltMusic();
}

void Audio::haltSoundEffects()
{
    Mix_HaltChannel(-1);
}

bool Audio::isMusicPlaying()
{
    return Mix_PlayingMusic() != 0;
}
