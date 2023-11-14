/**
 * @file config.h
 * @author Selkamies
 * 
 * @brief Holds global config values in a struct. The values are read from config.ini in config_handler.c.
 * 
 * @date Created 2023-11-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef CONFIG_H
#define CONFIG_H



struct Config
{
    int MAX_PIN_LENGTH;
    int KEYPRESS_TIMEOUT;
    int KEYPAD_ROWS;
    int KEYPAD_COLUMNS;
};

extern const Config config;

/** @brief Maximum length of the PIN. */
//extern const int MAX_PIN_LENGTH;
/** @brief Time in seconds after the last keypress, after which the program stops waiting and resets the PIN. */
//extern const int KEYPRESS_TIMEOUT;
/** @brief Number of rows in the keypad. */
//extern const int KEYPAD_ROWS;
/** @brief Number of columns in the keypad. */
//extern const int KEYPAD_COLUMNS;



#endif // CONFIG_H