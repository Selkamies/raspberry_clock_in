/**
 * @file sounds.h
 * @author Selkamies
 * 
 * @brief Handles playing sounds. 
 * 
 * @date Created 2023-11-24
 * @date Updated 2023-11-24
 * 
 * @copyright Copyright (c) 2023
 */



#ifndef SOUNDS_H
#define SOUNDS_H



enum Sound {
  BEEP_NORMAL,
  BEEP_SUCCESS,
  BEEP_ERROR
};

#define BEEP_NORMAL_FILE "./../sounds/beep_input.wav"
#define BEEP_SUCCESS_FILE "./../sounds/beep_success.wav"
#define BEEP_ERROR_FILE "./../sounds/beep_error.wav"



void initializeSounds();
void playSound(enum Sound sound);
void cleanupSounds();



#endif // SOUNDS_H