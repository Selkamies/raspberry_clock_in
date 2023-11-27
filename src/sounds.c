/**
 * @file sounds.h
 * @author Selkamies
 * 
 * @brief Handles playing sounds. 
 * 
 * @date Created 2023-11-24
 * @date Updated 2023-11-27
 * 
 * @copyright Copyright (c) 2023
 * 
 * TODO: Less magic numbers?
 * TODO: Get the default audio device (3) correctly.
 */


#include <stdio.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include <stdlib.h>         // For system(), which we use to play sound files.

#include "sounds.h"




Mix_Chunk *beepNormal = NULL;
Mix_Chunk *beepSuccess = NULL;
Mix_Chunk *beepError = NULL;



void initializeSounds()
{
    // Set the desired audio driver
    //setenv("SDL_AUDIODRIVER", "alsa", 1);  // Use "alsa" or "pulseaudio" depending on your setup

    int result = 0;
    int flags = MIX_INIT_MP3;

    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        printf("Failed to init SDL\n");
    }

    // Select first valid sound device.
    /* const char* validDeviceName = findValidSoundDevice();

    if (validDeviceName) 
    {
        // The valid device was found, use it
        printf("Using valid audio device: %s\n", validDeviceName);

        // Replace "hw:3,0" with the desired audio device
        if (Mix_OpenAudioDevice(44100, AUDIO_S16SYS, 2, 1024, validDeviceName, 0) < 0) 
        {
            printf("Could not open audio: %s\n", Mix_GetError());
        }
    } 
    
    else 
    {
        // Handle the case where no valid device is found
        printf("No valid audio device found\n");
    } */

    // List all audio devices.
    /* int numDevices = SDL_GetNumAudioDevices(0);  // 0 for playback devices

    for (int i = 0; i < numDevices; ++i) 
    {
        printf("Audio Device %d: %s\n", i, SDL_GetAudioDeviceName(i, 0));
    } */

    // Get default audio device.
    /* SDL_AudioSpec want, have;
    SDL_zero(want);
    SDL_zero(have);

    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (deviceId == 0) 
    {
        printf("Failed to open audio: %s\n", SDL_GetError());
    } 
    
    else 
    {
        printf("Default audio device: %s\n", SDL_GetAudioDeviceName(deviceId, 0));
        SDL_CloseAudioDevice(deviceId);
    } */

    if (flags != (result = Mix_Init(flags)))
    {
        printf("Could not initialize mixer (result: %d).\n", result);
        printf("Mix_Init: %s\n", Mix_GetError());
    }

    if (Mix_OpenAudioDevice(44100, AUDIO_S16SYS, 2, 1024, SDL_GetAudioDeviceName(AUDIO_DEVICE_INDEX, 0), 0) < 0)
    {
        printf("Could not open audio: %s\n", Mix_GetError());
    }

    // Allocate channels for sound effects.
    if (Mix_AllocateChannels(8) < 0)
    {
        printf("Could not allocate audio channels: %s\n", Mix_GetError());
    }

    // Load sound files into Mix_Chunk variables.
    beepNormal = Mix_LoadWAV(BEEP_NORMAL_FILE_PATH);
    beepSuccess = Mix_LoadWAV(BEEP_SUCCESS_FILE_PATH);
    beepError = Mix_LoadWAV(BEEP_ERROR_FILE_PATH);

    if (!beepNormal || !beepSuccess || !beepError)
    {
        printf("Failed to load sound effects: %s\n", Mix_GetError());
        SDL_Quit();
    }
}

/* const char* findValidSoundDevice() 
{
    int numDevices = SDL_GetNumAudioDevices(0);

    for (int i = 0; i < numDevices; ++i) 
    {
        const char* deviceName = SDL_GetAudioDeviceName(i, 0);
        SDL_AudioSpec want, have;
        SDL_zero(want);
        SDL_zero(have);

        SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(deviceName, 0, &want, &have, 0);

        if (deviceId != 0) 
        {
            printf("Opened audio device: %s\n", deviceName);
            SDL_CloseAudioDevice(deviceId);
            return deviceName;  // Return the name of the first opened device
        } 

        else 
        {
            printf("Failed to open audio device %s: %s\n", deviceName, SDL_GetError());
        }
    }

    // No valid device found
    return NULL;
} */

void playSound(enum Sound sound)
{
    if (sound == SOUND_BEEP_NORMAL)
    {
        // -q disables the print message when playing.
        // -D hw:3,0 uses third sound device.
        //system("aplay -q -D hw:3,0 " BEEP_NORMAL_FILE_PATH);
        Mix_PlayChannel(-1, beepNormal, 0);
    }

    else if (sound == SOUND_BEEP_SUCCESS)
    {
        //system("aplay -q -D hw:3,0 " BEEP_SUCCESS_FILE_PATH);
        Mix_PlayChannel(-1, beepSuccess, 0);
    }

    else if (sound == SOUND_BEEP_ERROR)
    {
        //system("aplay -q -D hw:3,0 " BEEP_ERROR_FILE_PATH);
        Mix_PlayChannel(-1, beepError, 0);
    }
}

void cleanupSounds()
{
    // Free loaded sound chunks
    Mix_FreeChunk(beepNormal);
    Mix_FreeChunk(beepSuccess);
    Mix_FreeChunk(beepError);

    Mix_Quit();
    SDL_Quit();
}