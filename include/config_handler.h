/**
 * @file config_handler.h
 * @author Selkamies
 * 
 * @brief Reads key-value pairs from config.ini and passes relevant values to other files.
 * 
 * @date Created 2023-11-15
 * @date Modified 2023-11-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */



#ifndef CONFIG_HANDLER_H
#define CONFIG_HANDLER_H



#include "config_data.h"



/**
 * @brief Struct holding all values read from config.ini. Relevant values are passed to files
 * that need them, and this struct is discarded after we have done passing them.
 * 
 */
/* struct ConfigData
{
    //////////////
    // keypad.c //
    //////////////

    struct KeypadConfig keypadConfig;
    struct KeypadGPIOPins keypadPins;
    // TODO: Two-dimensional malloc required.
    //struct Keypad keypadState;
    char **keypadKeys;
    
    ////////////
    // leds.c //
    ////////////
    
    //struct LEDGPIOPins LEDPins;
    //int LEDStaysOnFor;
    struct LEDConfig LEDConfigData;

    //////////////
    // sounds.c //
    //////////////

    int audioDeviceID;
}; */



/**
 * @brief Reads key-value pairs from config.ini and passes relevant values to other files.
 */
void readConfigFile(struct ConfigData *configData);



#endif // CONFIG_HANDLER_H