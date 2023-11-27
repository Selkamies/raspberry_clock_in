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
 * TODO: Use SDL2 and SDL2_mixer?
 * TODO: Get the default audio device (3) correctly.
 */


#include <stdio.h>
#include <stdlib.h>         // For system(), which we use to play sound files.

#include "sounds.h"



// TODO: Keep track if this has been initialized and initialize it when necessary?
void initializeSounds()
{

}

void playSound(enum Sound sound)
{
    if (sound == SOUND_BEEP_NORMAL)
    {
        // -q disables the print message when playing.
        // -D hw:3,0 uses third sound device.
        system("aplay -q -D hw:3,0 " BEEP_NORMAL_FILE_PATH);
    }

    else if (sound == SOUND_BEEP_SUCCESS)
    {
        system("aplay -q -D hw:3,0 " BEEP_SUCCESS_FILE_PATH);
    }

    else if (sound == SOUND_BEEP_ERROR)
    {
        system("aplay -q -D hw:3,0 " BEEP_ERROR_FILE_PATH);
    }
}

void cleanupSounds()
{
    
}