/**
 * @file sounds.h
 * @author Selkamies
 * 
 * @brief Handles playing sounds. 
 * 
 * @date Created 2023-11-24
 * @date Updated 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 * TODO: SDL2 and SDL_mixer use the zlib license. Do we need to mention this and where?
 * TODO: Default audio device seems to be 3.5 mm audio jack, even though it has nothing plugged in.
 *       Try to choose a device that actually works.
 * TODO: Less magic numbers?
 */



#include <stdio.h>              // printf().

#include "SDL2/SDL.h"
//#include "SDL2/SDL_mixer.h"     // SDL_mixer handles playing sound files. Needed here for Mix_Chunk.

#include "sounds.h"
#include "sounds_config.h"



#define SOUNDS_FOLDER_PATH "./../sounds/"
// For release?
//#define SOUNDS_FOLDER_PATH "./sounds/"

#define BEEP_NORMAL_FILE_NAME "beep_input.wav"
#define BEEP_SUCCESS_FILE_NAME "beep_success.wav"
#define BEEP_ERROR_FILE_NAME "beep_error.wav"

#define BEEP_NORMAL_FILE_PATH (SOUNDS_FOLDER_PATH BEEP_NORMAL_FILE_NAME)
#define BEEP_SUCCESS_FILE_PATH (SOUNDS_FOLDER_PATH BEEP_SUCCESS_FILE_NAME)
#define BEEP_ERROR_FILE_PATH (SOUNDS_FOLDER_PATH BEEP_ERROR_FILE_NAME)



#pragma region FunctionDeclarations

/**
 * @brief Selects either the default audio device or manually set audio device with specified id.
 * 
 * @param manualAudioDeviceID ID of the audio device to be used. -1 if SDL chooses default.
 * 
 * @return SDL_AudioDeviceID The SDL audio device id number selected. integer.
 */
static SDL_AudioDeviceID selectAudioDeviceID(const int manualAudioDeviceID);

#pragma endregion // FunctionDeclarations



void playSound(const struct SoundsConfig *soundsConfig, enum Sound sound)
{
    if (sound == SOUND_BEEP_NORMAL)
    {
        // -q disables the print message when playing.
        // -D hw:3,0 uses third sound device.
        //system("aplay -q -D hw:3,0 " BEEP_NORMAL_FILE_PATH);

        Mix_PlayChannel(-1, soundsConfig->sounds.beepNormal, 0);
    }

    else if (sound == SOUND_BEEP_SUCCESS)
    {
        Mix_PlayChannel(-1, soundsConfig->sounds.beepSuccess, 0);
    }

    else if (sound == SOUND_BEEP_ERROR)
    {
        Mix_PlayChannel(-1, soundsConfig->sounds.beepError, 0);
    }
}



void initializeSounds(struct SoundsConfig *soundsConfig)
{
    printf("Initializing sounds.\n");

    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        printf("Failed to init SDL\n");
    }

    SDL_AudioDeviceID deviceId = selectAudioDeviceID(soundsConfig->manualAudioDeviceID);

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
    soundsConfig->sounds.beepNormal = Mix_LoadWAV(BEEP_NORMAL_FILE_PATH);
    soundsConfig->sounds.beepSuccess = Mix_LoadWAV(BEEP_SUCCESS_FILE_PATH);
    soundsConfig->sounds.beepError = Mix_LoadWAV(BEEP_ERROR_FILE_PATH);

    if (!soundsConfig->sounds.beepNormal || !soundsConfig->sounds.beepSuccess || !soundsConfig->sounds.beepError)
    {
        printf("SDL_mixer failed to load sound files: %s\n", Mix_GetError());
        SDL_Quit();
    }
}

static SDL_AudioDeviceID selectAudioDeviceID(const int manualAudioDeviceID)
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

void cleanupSounds(struct SoundsConfig *soundsConfig)
{
    // Free loaded sound chunks.
    Mix_FreeChunk(soundsConfig->sounds.beepNormal);
    Mix_FreeChunk(soundsConfig->sounds.beepSuccess);
    Mix_FreeChunk(soundsConfig->sounds.beepError);

    Mix_Quit();
    SDL_Quit();
}