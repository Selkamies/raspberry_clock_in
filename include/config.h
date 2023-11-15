/**
 * @file config.h
 * @author Selkamies
 * 
 * @brief Holds global config values in a struct. The values are read from config.ini in config_handler.c.
 * 
 * @date Created 2023-11-14
 * @date Modified 2023-11-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef CONFIG_H
#define CONFIG_H



/**
 * @brief Struct that holds config variables.
 */
struct Config
{
    int MAX_PIN_LENGTH;
    int KEYPRESS_TIMEOUT;
    int KEYPAD_ROWS;
    int KEYPAD_COLUMNS;
};

/** @brief Struct that holds config variables. */
extern struct Config config;



#endif // CONFIG_H