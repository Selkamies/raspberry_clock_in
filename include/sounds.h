/**
 * @file sounds.h
 * @author Selkamies
 * 
 * @brief Handles playing sounds. 
 * 
 * @date Created 2023-11-24
 * @date Updated 2023-12-04
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