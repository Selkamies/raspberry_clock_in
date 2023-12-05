/**
 * @file leds_config.h
 * @author Selkamies
 * 
 * @brief 
 * 
 * @date Created 2023-12-05
 * @date Modified 2023-12-05
 * 
 * @copyright Copyright (c) 2023
 */



#ifndef CONFIG_DATA_H
#define CONFIG_DATA_H



#include "keypad.h"
#include "leds_config.h"

// Forward declaration to avoid circular dependency
//struct KeypadConfig;
//struct KeypadGPIOPins;
//struct LEDConfig;

struct ConfigData
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
};



#endif  // CONFIG_DATA_H