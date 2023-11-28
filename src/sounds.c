/**
 * @file sounds.h
 * @author Selkamies
 * 
 * @brief Handles playing sounds. 
 * 
 * @date Created 2023-11-24
 * @date Updated 2023-11-28
 * 
 * @copyright Copyright (c) 2023
 * 
 * TODO: SDL2 and SDL_mixer use the zlib license. Do we need to mention this and where?
 * TODO: Default audio device seems to be 3.5 mm audio jack, even though it has nothing plugged in.
 *       Try to choose a device that actually works.
 * TODO: Sound device id from config.ini?
 * TODO: Less magic numbers?
 */



#include <stdio.h>

#include "SDL2/SDL.h"           

#include "sounds.h"



/** @brief Struct holding SDL_mixer sound chunks for the different sounds. */
struct SoundChunks sounds;

int manualAudioDeviceID;



void initializeSounds()
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        printf("Failed to init SDL\n");
    }

    SDL_AudioDeviceID deviceId = selectAudioDeviceID();

    int result = 0;
    // Flags used when initializing SDL_mixer.
    int flags = MIX_INIT_MP3;

    if (flags != (result = Mix_Init(flags)))
    {
        printf("Could not initialize SDL_mixer (result: %d).\n", result);
        printf("Mix_Init: %s\n", Mix_GetError());
    }

    if (Mix_OpenAudioDevice(44100, AUDIO_S16SYS, 2, 1024, SDL_GetAudioDeviceName(deviceId, 0), 0) < 0)
    {
        printf("SDL_mixer could not open audio: %s\n", Mix_GetError());
    }

    // Allocate channels for sound effects.
    if (Mix_AllocateChannels(8) < 0)
    {
        printf("SDL_mixer could not allocate audio channels: %s\n", Mix_GetError());
    }

    // Load sound files into Mix_Chunk variables.
    sounds.beepNormal = Mix_LoadWAV(BEEP_NORMAL_FILE_PATH);
    sounds.beepSuccess = Mix_LoadWAV(BEEP_SUCCESS_FILE_PATH);
    sounds.beepError = Mix_LoadWAV(BEEP_ERROR_FILE_PATH);

    if (!sounds.beepNormal || !sounds.beepSuccess || !sounds.beepError)
    {
        printf("SDL_mixer failed to load sound files: %s\n", Mix_GetError());
        SDL_Quit();
    }
}

void setSoundsConfig(int manualDeviceID)
{
    manualAudioDeviceID = manualDeviceID;
}

SDL_AudioDeviceID selectAudioDeviceID()
{
    SDL_AudioDeviceID deviceId;

    // -1 for audio device id in config.ini, let SDL select default audio device.
    if (manualAudioDeviceID == -1)
    {
        SDL_AudioSpec want, have;
        SDL_zero(want);
        SDL_zero(have);
        deviceId = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    }

    // Otherwise, the user has manually specified audio device id in config.ini.
    else
    {
        deviceId = manualAudioDeviceID;
    }

    if (deviceId == 0) 
    {
        printf("SDL2 failed to open audio: %s\n", SDL_GetError());
    } 
    
    else 
    {
        if (manualAudioDeviceID == -1)
        {
            printf("SDL2 default audio device: %s\n", SDL_GetAudioDeviceName(deviceId, 0));
        }

        else
        {
            printf("SDL2 manual audio device: %s\n", SDL_GetAudioDeviceName(deviceId, 0));
        }

        SDL_CloseAudioDevice(deviceId);
    }

    return deviceId;
}



void playSound(enum Sound sound)
{
    if (sound == SOUND_BEEP_NORMAL)
    {
        // -q disables the print message when playing.
        // -D hw:3,0 uses third sound device.
        //system("aplay -q -D hw:3,0 " BEEP_NORMAL_FILE_PATH);

        Mix_PlayChannel(-1, sounds.beepNormal, 0);
    }

    else if (sound == SOUND_BEEP_SUCCESS)
    {
        Mix_PlayChannel(-1, sounds.beepSuccess, 0);
    }

    else if (sound == SOUND_BEEP_ERROR)
    {
        Mix_PlayChannel(-1, sounds.beepError, 0);
    }
}

void cleanupSounds()
{
    // Free loaded sound chunks.
    Mix_FreeChunk(sounds.beepNormal);
    Mix_FreeChunk(sounds.beepSuccess);
    Mix_FreeChunk(sounds.beepError);

    Mix_Quit();
    SDL_Quit();
}