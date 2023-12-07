/**
 * @file sounds_config.h
 * @author Selkamies
 * 
 * @brief 
 * 
 * @date Created 2023-12-07
 * @date Updated 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 */



#ifndef SOUNDS_CONFIG_H
#define SOUNDS_CONFIG_H



#include "SDL2/SDL_mixer.h"     // SDL_mixer handles playing sound files. Needed here for Mix_Chunk.



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

struct SoundsConfig
{
    struct SoundChunks sounds;
    int manualAudioDeviceID;
};



#endif // SOUNDS_CONFIG_H