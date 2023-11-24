/**
 * @file sounds.h
 * @author Selkamies
 * 
 * @brief Handles playing sounds. 
 * 
 * @date Created 2023-11-24
 * @date Updated 2023-11-24
 * 
 * @copyright Copyright (c) 2023
 */



#include <stdlib.h>         // For system(), which we use to play sound files.

#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"

#include "sounds.h"



// Define the number of available channels
#define NUM_CHANNELS 8



// TODO: Keep track if this has been initialized and initialize it when necessary?
void initializeSounds()
{
    int result = 0;
    int flags = MIX_INIT_MP3;

    if (SDL_Init(SDL_INIT_AUDIO) < 0) 
    {
        printf("Failed to init SDL\n");
    }

    printf("SDL_Init result: %d\n", SDL_Init(SDL_INIT_AUDIO));
    printf("Mix_Init result: %d\n", Mix_Init(flags));
    printf("Mix_OpenAudio result: %d\n", Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024));

    if (flags != (result = Mix_Init(flags))) 
    {
        printf("Could not initialize mixer (result: %d).\n", result);
        printf("Mix_Init: %s\n", Mix_GetError());
    }

    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) < 0)
    {
        printf("Could not open audio: %s\n", Mix_GetError());
    }

    // Allocate channels for sound effects
    if (Mix_AllocateChannels(NUM_CHANNELS) < 0) 
    {
        printf("Could not allocate audio channels: %s\n", Mix_GetError());
    }

    printf("SDL_GetError: %s\n", SDL_GetError());
    printf("Mix_GetError: %s\n", Mix_GetError());
}

void playSound(enum Sound sound)
{
    if (sound == BEEP_NORMAL)
    {
        printf("Playing sound.\n");

        Mix_Chunk *beep = Mix_LoadWAV(BEEP_NORMAL_FILE);

        if (!beep) 
        {
            printf("Failed to load sound effect: %s\n", Mix_GetError());
        }

        // Play the sound effect on the first available channel
        //int channel = Mix_PlayChannel(-1, beep, 0);
        int channel = Mix_PlayChannel(1, beep, 0);

        if (channel == -1) 
        {
            printf("Failed to play sound effect: %s\n", Mix_GetError());
        }

        SDL_Delay(500);
        printf("SUCCESS.\n");

        //system("aplay ./../sounds/beep_input.mp3");
    }

    else if (sound == BEEP_SUCCESS)
    {
        //system("aplay ./../sounds/beep_success.mp3");
    }

    else if (sound == BEEP_ERROR)
    {
        //system("aplay ./../sounds/beep_error.mp3");
    }
}

void cleanupSounds()
{
    SDL_Quit();
}