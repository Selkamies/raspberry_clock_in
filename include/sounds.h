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
 */



#ifndef SOUNDS_H
#define SOUNDS_H



// Forward declaration.
struct SoundsConfig;



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
void initializeSounds(struct SoundsConfig *soundsConfig);

/**
 * @brief Plays a desired sound.
 * 
 * @param sound Enumeration of the sound type to play. SOUND_BEEP_NORMAL, etc.
 */
void playSound(const struct SoundsConfig *soundsConfig, enum Sound sound);

/**
 * @brief Cleans up any resourced used by sound.c.
 */
void cleanupSounds(struct SoundsConfig *soundsConfig);



#endif // SOUNDS_H