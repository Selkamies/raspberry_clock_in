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



// Forward declarations.
struct ConfigData;
struct KeypadConfig;



/**
 * @brief Updates the keypad status and handles key presses if necessary.
 */
void updateKeypad(struct ConfigData *configData);



/**
 * @brief Initializes all arrays used by keypad requiring malloc/calloc.
 */
void initializeKeypad(struct KeypadConfig *keypadConfig);

/**
 * @brief Frees all arrays used by keypad requiring malloc/calloc.
 */
void cleanupKeypad(struct KeypadConfig *keypadConfig);



#endif // KEYPAD_H