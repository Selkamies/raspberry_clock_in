/**
 * @file leds_config.h
 * @author Selkamies
 * 
 * @brief 
 * 
 * @date Created 2023-12-05
 * @date Modified 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 */



#ifndef CONFIG_DATA_H
#define CONFIG_DATA_H



#include "keypad.h"
#include "leds_config.h"
#include "sounds_config.h"



struct ConfigData
{
    //////////////
    // keypad.c //
    //////////////

    struct KeypadConfig keypadConfig;
    struct KeypadGPIOPins keypadPins;
    char **keypadKeys;
    
    ////////////
    // leds.c //
    ////////////
    
    struct LEDConfig LEDConfigData;

    //////////////
    // sounds.c //
    //////////////

    struct SoundsConfig soundsConfig;
    //int audioDeviceID;
};



#endif  // CONFIG_DATA_H