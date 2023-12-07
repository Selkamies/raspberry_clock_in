/**
 * @file main.c
 * @author Selkamies
 * 
 * @brief Clock-in application for Raspberry Pi 4. 
 * User enters PIN, and relevant user is then marked as present or not, depending if
 * they were already marked as present or not. Users and logs are stored in a database.
 * 
 * @date Created  2023-11-13
 * @date Modified 2023-12-07
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

//#include "keypad_config.h"      // struct KeypadConfig.
//#include "leds_config.h"        // struct LEDConfig.
//#include "sounds_config.h"      // struct SoundsConfig.



void mainLoop(struct ConfigData *configData)
{
    printf("\nMain loop starting. You may now input PIN.\n\n");

    // TODO: structs that hold all the globals in keypad.c and leds.c.

    // CTRL-C will end he main loop.
    while (!signal_received) 
    {
        updateKeypad(configData);
        updateLED(&configData->LEDConfigData);

        sleepGPIOLibrary(0.01);
    }
}



/**
 * @brief Read files, set up variables, start pigpio.
 */
void initialize(struct ConfigData *configData)
{
    if (!initializeGPIOLibrary())
    {
        return;
    }

    readConfigFile(configData);

    initializeKeypad(&configData->keypadConfig);
    initializeLeds(&configData->LEDConfigData);
    initializeSounds(&configData->soundsConfig);
}

/**
 * @brief Freeing memory, turning off leds.
 */
void cleanup(struct ConfigData *configData)
{
    turnLEDsOff(&configData->LEDConfigData);

    cleanupKeypad(&configData->keypadConfig);
    cleanupSounds(&configData->soundsConfig);

    cleanupGPIOLibrary();
}



int main()
{
    printf("\nProgram starting.\n");

    // Struct holding basically all variables used by the program.
    struct ConfigData configData;

    initialize(&configData);
    mainLoop(&configData);
    cleanup(&configData);

    return 0;
}

