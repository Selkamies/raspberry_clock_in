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
 */



#ifndef SOUNDS_H
#define SOUNDS_H



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
 * @brief The index number of the audio device that is used to play the sounds.
 * TODO: Read from file or actually get the default sound device selection to work.
 */
#define AUDIO_DEVICE_INDEX 3

/** @brief File path and file name of normal beep sound file.
 * TODO: This is different for debug and release. Read from config file?
 */
#define BEEP_NORMAL_FILE_PATH "./../sounds/beep_input.wav"
#define BEEP_SUCCESS_FILE_PATH "./../sounds/beep_success.wav"
#define BEEP_ERROR_FILE_PATH "./../sounds/beep_error.wav"



/**
 * @brief Initializes anything required to play sounds. Currently SDL2 and SDL2_Mixer.
 */
void initializeSounds();

//const char* findValidSoundDevice();

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