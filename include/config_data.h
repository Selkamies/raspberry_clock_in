/**
 * @file config_data.h
 * @author Selkamies
 * 
 * @brief Defines the ConfigData struct, which is used to hold basically all variables used by the program.
 * ConfigData has substructs for separating the data used by keypad, leds and sounds.
 * 
 * @date Created 2023-12-05
 * @date Modified 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 */



#ifndef CONFIG_DATA_H
#define CONFIG_DATA_H


#include <sqlite3.h>             // sqlite3.

// Cannot forward declare?
#include "keypad_config.h"
#include "leds_config.h"
#include "sounds_config.h"



/**
 * @brief Struct holding data about basically all variables used by the program.
 */
struct ConfigData
{
    /** @brief Struct holding configuration variables used by keypad and PIN reading. */
    struct KeypadConfig keypadConfig;
    /** @brief Struct holding all the variables needed by leds.c. */
    struct LEDConfig LEDConfigData;
    /** @brief Struct holding all the variables needed by sounds.c. */
    struct SoundsConfig soundsConfig;
    sqlite3 **database;
};



#endif  // CONFIG_DATA_H