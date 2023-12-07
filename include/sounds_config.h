/**
 * @file sounds_config.h
 * @author Selkamies
 * 
 * @brief Defines SoundsConfig struct, which holds basically all data used by the sounds.c.
 * 
 * @date Created 2023-12-07
 * @date Updated 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 */



#ifndef SOUNDS_CONFIG_H
#define SOUNDS_CONFIG_H



#include "SDL2/SDL_mixer.h"     // Mix_Chunk. Cannot forward declare?



/**
 * @brief Struct holding SDL_mixer sound chunks for the different sounds.
 */
struct SoundChunks
{
    /** @brief .wav file loaded to chunk used by SDL_mixer. Used for normal beep when pressing keypad numbers. */
    Mix_Chunk *beepNormal;
    /** @brief .wav file loaded to chunk used by SDL_mixer. Used after correct PIN has been entered. */
    Mix_Chunk *beepSuccess;
    /** @brief .wav file loaded to chunk used by SDL_mixer. Used for after timeout or wrong PIN has been entered. */
    Mix_Chunk *beepError;
};

/**
 * @brief Struct holding all the variables needed by sounds.c.
 */
struct SoundsConfig
{
    /** @brief Struct holding SDL_mixer sound chunks for the different sounds. */
    struct SoundChunks sounds;
    /** @brief Holds the ID of a sound device read from config.ini. If -1, SDL uses default device. */
    int manualAudioDeviceID;
};



#endif // SOUNDS_CONFIG_H