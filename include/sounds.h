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



enum Sound 
{
  SOUND_BEEP_NORMAL,
  SOUND_BEEP_SUCCESS,
  SOUND_BEEP_ERROR
};

#define BEEP_NORMAL_FILE_PATH "./../sounds/beep_input.wav"
#define BEEP_SUCCESS_FILE_PATH "./../sounds/beep_success.wav"
#define BEEP_ERROR_FILE_PATH "./../sounds/beep_error.wav"



void initializeSounds();
void playSound(enum Sound sound);
void cleanupSounds();



#endif // SOUNDS_H