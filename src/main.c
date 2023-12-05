/**
 * @file main.c
 * @author Selkamies
 * 
 * @brief Clock-in application for Raspberry Pi 4. 
 * User enters PIN, and relevant user is then marked as present or not, depending if
 * they were already marked as present or not. Users and logs are stored in a database.
 * 
 * @date Created  2023-11-13
 * @date Modified 2023-12-05
 * 
 * @copyright Copyright (c) 2023
 * 
 * TODO: Read files to a temporary config struct in initialize() here from config.ini,
 *       then pass the config struct to initialize() functions in keypad.c and leds.c.
 *       Temporary config struct gets discared after initialize().
 *       Create file structs 
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



void mainLoop(struct LEDConfig *LEDConfigData)
{
    printf("\nMain loop starting. You may now input PIN.\n\n");

    // TODO: structs that hold all the globals in keypad.c and leds.c.

    // CTRL-C will end he main loop.
    while (!signal_received) 
    {
        updateKeypad(LEDConfigData);
        updateLED(LEDConfigData);

        sleepGPIOLibrary(0.01);
    }
}



/**
 * @brief Read files, set up variables, start pigpio.
 */
void initialize(struct LEDConfig *LEDConfigData)
{
    if (!initializeGPIOLibrary())
    {
        return;
    }

    struct ConfigData tempConfigData;
    // Config file has to be read before we initialize keypad.
    readConfigFile(&tempConfigData);
    initializeKeypad();
    initializeLeds(&tempConfigData, LEDConfigData);
    initializeSounds();
}

/**
 * @brief Freeing memory, turning off leds.
 */
void cleanup(struct LEDConfig *LEDConfigData)
{
    cleanupKeypad(LEDConfigData);
    cleanupSounds();

    cleanupGPIOLibrary();
}



int main()
{
    printf("\nProgram starting.\n");

    struct LEDConfig LEDConfigData;

    initialize(&LEDConfigData);
    mainLoop(&LEDConfigData);
    cleanup(&LEDConfigData);

    return 0;
}

