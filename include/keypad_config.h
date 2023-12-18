/**
 * @file keypad_config.h
 * @author Selkamies
 * 
 * @brief Defines KeypadConfig struct, which holds basically all data used by the keypad.c.
 * 
 * @date Created  2023-12-07
 * @date Modified 2023-12-18
 * 
 * @copyright Copyright (c) 2023
 * 
 * TODO: KEYPRESS_TIMEOUT as double?
 */



#ifndef KEYPAD_CONFIG_H
#define KEYPAD_CONFIG_H



#include <stdbool.h>



/**
 * @brief Struct holding the keys on the keypad and the previous state of the keypad keys.
 */
struct KeypadState
{
    /** @brief Holds the keys in the keypad, in the corresponding positions. */
    char **keys;
    /** @brief State of keys (pressed or not) during previous check. 
     * Used to check for changes, so that we only register button press once. */
    bool **keysPressedPreviously;
    /** @brief Current keypad key that is pressed. */
    char keyPressed;
    /** @brief Whether exactly one keypad key is pressed during a keypad update. */
    bool exactlyOneKeyPressed;
    /** @brief Whether any keypad key is pressed during a keypad update. */
    bool anyKeysPressed;
    /** @brief Whether no keypad keys were pressed during a previous keypad update. */
    bool noKeysPressedPreviously;
    /** @brief Time when last keypad update was done. */
    double lastUpdateTime;
    /** @brief Key used to start waiting for PIN for clocking IN. */
    char clockInKey;
    /** @brief Key used to start waiting for PIN for clocking OUT. */
    char clockOutKey;
};

/**
 * @brief Struct holding data about the PIN the user is currently trying to input, if any.
 */
struct PINState
{
    /** @brief Index for the next free char in the array holding the PIN being entered. */
    int nextPressIndex;
    /** @brief Whether key was recently pressed and we are counting until timeout. */
    bool lastPressTimerOn;
    /** @brief Time when the last key was pressed. */
    double lastKeyPressTime;
    /** @brief Array holding the PIN being entered. */
    char *keyPresses;
    /** @brief Whether we are currently waiting for PIN input. If not, we only check for clockInKey or clockOutKey. */
    bool waitingForPINInput;
    /** @brief The status code of the current PIN input. Clocking IN (1) or OUT (2). */
    int status;
};

/**
 * @brief Struct holding the pin numbers for all Raspberry Pi 4 GPIO pins used by the program.
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
    /** @brief Maximum PIN length in characters/numbers. */
    int MAX_PIN_LENGTH;
    /** @brief Time in seconds after the PIN is reset if we don'g get more input. */
    int KEYPRESS_TIMEOUT;
    /** @brief Number of rows in the keypad. */
    int KEYPAD_ROWS;
    /** @brief Number of columns in the keypad. */
    int KEYPAD_COLUMNS;
    /** @brief Minimum time between keypad updates in seconds. */
    double UPDATE_INTERVAL_SECONDS;

    /** @brief Struct holding the keys on the keypad and the previous state of the keypad keys. */
    struct KeypadState keypadState;
    /** @brief Struct holding data about the PIN the user is currently trying to input, if any. */
    struct PINState currentPINState;
    /** @brief Struct holding the pin numbers for all Raspberry Pi 4 GPIO pins used by the program. */
    struct KeypadGPIOPins pins;
};



#endif // KEYPAD_CONFIG_H