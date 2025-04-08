#ifndef AUDIO_H
#define AUDIO_H

#include <SDL_mixer.h>
#include <string>
#include "defs.h"

using namespace std;

struct Audio
{
    // Initialize
    Mix_Music* bgMusic = nullptr;
    Mix_Chunk* catchSound = nullptr;
    Mix_Chunk* gameOverSound = nullptr;

    // Functions
    void logErrorAndExit(const char* msg, const char* error);
    void init();
    void loadSounds();
    void destroy();
    void play(Mix_Music* music);
    void play(Mix_Chunk* chunk);
    void pauseMusic();
    void resumeMusic();
    void haltMusic();
    void haltSoundEffects();
    bool isMusicPlaying();
};

#endif // AUDIO_H
