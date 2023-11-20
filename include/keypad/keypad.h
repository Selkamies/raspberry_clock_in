/**
 * @file keypad.h
 * @author Selkamies
 * 
 * @brief Handles the input from a keypad attached to Raspberry Pi 4. 
 * This file contains the logic, all GPIO pin handling by pigpio is in keypad_gpio.h.
 * 
 * @date Created  2023-11-13
 * @date Modified 2023-11-20
 * 
 * @copyright Copyright (c) 2023
 */



#ifndef KEYPAD_H
#define KEYPAD_H



#include <stdbool.h>
#include <time.h>



/** @brief Default value used for empty characters when no character is received or set. */
#define EMPTY_KEY '\0'
/** @brief Default value used for timestamps when no time is recorded. */
#define EMPTY_TIMESTAMP 0



// Forward declaration.
struct GPIOPins;

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
 * @brief Struct holding data about the PIN the user is currently trying to input, if any.
 */
struct CurrentPinInput
{
    /** @brief Index for the next free char in the array holding the PIN being entered. */
    int nextPressIndex;
    /** @brief Whether key was recently pressed and we are counting until timeout. */
    bool lastPressTimerOn;
    /** @brief Time since the last key was pressed. */
    time_t lastKeyPressTime;
    /** @brief Array holding the PIN being entered. */
    char *keyPresses;
};

/**
 * @brief Struct holding the keys on the keypad and the previous state of the keypad keys.
 */
struct Keypad
{
    /** @brief Holds the keys in the keypad, in the corresponding positions. */
    char **keys;
    /** @brief State of keys (pressed or not) during previous check. 
     * Used to check for changes, so that we only register button press once. */
    bool **keysPressedPreviously;
};



/**
 * @brief Checks if any key in the keypad was just pressed, and handles it accordingly.
 */
void updateKeypad();

/**
 * @brief Stores the pressed key to the current PIN under input.
 * 
 * @param key Key to save.
 */
void storeKeyPress(char key);

/**
 * @brief Resets the currently input PIN.
 */
void clearPIN();

/**
 * @brief Checks the full PIN for validity. Currently mock checks.
 * TODO: Check database for existing person that has the passed PIN.
 * 
 * @param pin_input The PIN to check.
 * @return true If the PIN matches.
 * @return false If the PIN doesn't have a match.
 */
bool checkPIN(char *pin_input);

/**
 * @brief Starts the timer counting the time since last keypress by recordin current time.
 * Called at every keypad key press.
 */
void startTimeoutTimer();

/**
 * @brief Resets/stops the timer counting the time since last keypress.
 * Called at initialization, when the timer reaches defined time limit, or when PIN is rejected or accepted.
 */
void resetTimeoutTimer();

/**
 * @brief Checks if it has been too long since the last keypress. If yes, rejects the PIN.
 * @return true If it has been too long.
 * @return false If it hasn't been too long.
 */
bool tooLongSinceLastKeypress();

/**
 * @brief Prints a 4x4 array of zeroes and ones represeting the keypad key status. 
 * Ones marking a key in the keypad being currently pressed down.
 */
void printKeyStatus();



/**
 * @brief Set the values that are read from file by config_handler and are needed by keypad.
 * 
 * @param keyPins Struct holding the pin numbers for each row and column of the keypad.
 */
void setKeypadValues(struct KeypadGPIOPins *keyPins);

/**
 * @brief Initializes all arrays used by keypad requiring malloc/calloc.
 */
void initializeKeypad();

/**
 * @brief Frees all arrays used by keypad requiring malloc/calloc.
 */
void cleanupKeypad();



#endif // KEYPAD_H