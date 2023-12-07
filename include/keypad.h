/**
 * @file keypad.h
 * @author Selkamies
 * 
 * @brief Handles the input from a keypad attached to Raspberry Pi 4. 
 * This file contains the logic, all GPIO pin handling by pigpio is in keypad_gpio.h.
 * 
 * @date Created  2023-11-13
 * @date Modified 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 */



#ifndef KEYPAD_H
#define KEYPAD_H



#include "leds_config.h"
#include "sounds_config.h"



/**
 * @brief Struct holding the pin numbers for all Raspberry Pi 4 GPIO pins used by the program.
 * TODO: Read the pin numbers from file, so we don't have to recompile if we change pins.
 */
struct KeypadGPIOPins
{
    /** @brief GPIO pin numbers of the keypad rows. */
    int *keypad_rows;
    /** @brief GPIO pin numbers of the keypad columns. */
    int *keypad_columns;
};

/**
 * @brief Struct holding configuration variables used by keypad and PIN reading.
 */
struct KeypadConfig
{
    /** @brief Maximum PIN length. */
    int MAX_PIN_LENGTH;
    /** @brief Time in seconds after the PIN is reset if we don'g get more input. */
    int KEYPRESS_TIMEOUT;
    /** @brief Number of rows in the keypad. */
    int KEYPAD_ROWS;
    /** @brief Number of columns in the keypad. */
    int KEYPAD_COLUMNS;
    /** @brief Minimum time between keypad updates in seconds. */
    double UPDATE_INTERVAL_SECONDS;
};



/**
 * @brief Updates the keypad status and handles key presses if necessary.
 */
void updateKeypad(struct LEDConfig *LEDConfigData, struct SoundsConfig *soundsConfig);



/**
 * @brief Returns the current time in seconds using Linux time.
 * Does some minor math to get the total time in seconds.
 *  
 * Example: fullSeconds + (extraNanoSeconds / 1e9) = finalTime
 *                    5 + (350000000 / 1000000000) = 5.35.
 * 
 * @return double Current time in seconds.
 */
double getCurrentTimeInSeconds();



/**
 * @brief Set the values that are read from file by config_handler and are needed by keypad.
 * 
 * @param config Struct holding configuration variables used by keypad and PIN reading.
 * @param keyPins Struct holding the pin numbers for each row and column of the keypad.
 */
void setKeypadValues(struct KeypadConfig *config, struct KeypadGPIOPins *keyPins, char **keypadKeys);

/**
 * @brief Initializes all arrays used by keypad requiring malloc/calloc.
 */
void initializeKeypad();

/**
 * @brief Frees all arrays used by keypad requiring malloc/calloc.
 */
void cleanupKeypad(struct LEDConfig *LEDConfigData);



/**
 * @brief Prints a 4x4 array of zeroes and ones represeting the keypad key status. 
 * Ones marking a key in the keypad being currently pressed down.
 */
void printKeyStatus();



#endif // KEYPAD_H