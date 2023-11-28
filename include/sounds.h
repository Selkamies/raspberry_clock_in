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
 */



#ifndef SOUNDS_H
#define SOUNDS_H



#include "SDL2/SDL_mixer.h"     // SDL_mixer handles playing sound files. Needed here for Mix_Chunk.



/** 
 * @brief Enumeration of sounds used in the program. Passed to playSound() function as a parameter. 
 */
enum Sound 
{
    SOUND_BEEP_NORMAL,
    SOUND_BEEP_SUCCESS,
    SOUND_BEEP_ERROR
};

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



#define SOUNDS_FOLDER_PATH "./../sounds/"
// For release?
//#define SOUNDS_FOLDER_PATH "./sounds/"

#define BEEP_NORMAL_FILE_NAME "beep_input.wav"
#define BEEP_SUCCESS_FILE_NAME "beep_success.wav"
#define BEEP_ERROR_FILE_NAME "beep_error.wav"

#define BEEP_NORMAL_FILE_PATH (SOUNDS_FOLDER_PATH BEEP_NORMAL_FILE_NAME)
#define BEEP_SUCCESS_FILE_PATH (SOUNDS_FOLDER_PATH BEEP_SUCCESS_FILE_NAME)
#define BEEP_ERROR_FILE_PATH (SOUNDS_FOLDER_PATH BEEP_ERROR_FILE_NAME)



/**
 * @brief Initializes anything required to play sounds. Currently SDL2 and SDL2_Mixer.
 */
void initializeSounds();

/**
 * @brief Used to set the manualAudioDeviceId that is read from file. Called by config_handler.c.
 * 
 * @param manualDeviceID The audio device id the program will use. -1 means the default audio device will be used.
 */
void setSoundsConfig(int manualDeviceID);

/**
 * @brief Selects either the default audio device or manually set audio device with specified id.
 * 
 * @return SDL_AudioDeviceID The SDL audio device id number selected. integer.
 */
SDL_AudioDeviceID selectAudioDeviceID();

/**
 * @brief Plays a desired sound.
 * 
 * @param sound Enumeration of the sound type to play. SOUND_BEEP_NORMAL, etc.
 */
void playSound(enum Sound sound);

/**
 * @brief Cleans up any resourced used by sound.c.
 */
void cleanupSounds();



#endif // SOUNDS_H