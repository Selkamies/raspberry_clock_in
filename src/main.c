/**
 * @file main.c
 * @author Selkamies
 * 
 * @brief Clock-in application for Raspberry Pi 4. 
 * User enters PIN, and relevant user is then marked as present or not, depending if
 * they were already marked as present or not. Users and logs are stored in a database.
 * 
 * @date Created  2023-11-13
 * @date Modified 2023-12-01
 * 
 * @copyright Copyright (c) 2023
 * 
 * Possible future implementations:
 * TODO: Support for RFID tags.
 * TODO: Support for a touch screen.
 */



#include <stdio.h>              // printf().

#include "gpio_init.h"          // initializeGPIOLibrary(), sleepGPIOLibrary(), cleanupGPIOLibrary().
#include "config_handler.h"     // readConfigFile().
#include "keypad.h"             // initializeKeypad(), updatekeypad(), cleanupKeypad().
#include "leds.h"               // initializeLeds(), updateLED().
#include "sounds.h"             // initializeSounds(), cleanupSounds().



void mainLoop()
{
    printf("\nMain loop starting. You may now input PIN.\n\n");

    // CTRL-C will end he main loop.
    while (!signal_received) 
    {
        updateKeypad();
        updateLED();

        sleepGPIOLibrary(0.01);
    }
}



/**
 * @brief Read files, set up variables, start pigpio.
 */
void initialize()
{
    if (!initializeGPIOLibrary())
    {
        return;
    }

    // Config file has to be read before we initialize keypad.
    readConfigFile();
    initializeKeypad();
    initializeLeds();
    initializeSounds();
}

/**
 * @brief Freeing memory, turning off leds.
 */
void cleanup()
{
    cleanupKeypad();
    cleanupSounds();

    cleanupGPIOLibrary();
}



int main()
{
    printf("\nProgram starting.\n");

    initialize();
    mainLoop();
    cleanup();

    return 0;
}

