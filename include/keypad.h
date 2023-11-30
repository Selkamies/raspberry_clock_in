/**
 * @file keypad.h
 * @author Selkamies
 * 
 * @brief Handles the input from a keypad attached to Raspberry Pi 4. 
 * This file contains the logic, all GPIO pin handling by pigpio is in keypad_gpio.h.
 * 
 * @date Created  2023-11-13
 * @date Modified 2023-11-30
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
    double lastKeyPressTime;
    //time_t lastKeyPressTime;
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
    /** @brief Minimum time between keypad updates. */
    double updateInterval;
};



/**
 * @brief Updates the keypad status and handles key presses if necessary.
 */
void updateKeypad();

/**
 * @brief Loops through the keypad and updates a struct holding data about it's current status,
 * like whether any keys are pressed, what key is pressed, etc.
 */
void updateKeypadStatus();

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
bool validPIN(char *pin_input);

/**
 * @brief Starts the timer counting the time since last keypress by recordin current time.
 * Called at every keypad key press.
 */
void startTimeoutTimer();

/**
 * @brief Stops the timer counting the time since last keypress.
 * Called at initialization, when the timer reaches defined time limit, or when PIN is rejected or accepted.
 */
void stopTimeoutTimer();

/**
 * @brief Checks if it has been too long since the last keypress.
 * @return true If it has been too long.
 * @return false If it hasn't been too long.
 */
bool tooLongSinceLastKeypress();

/**
 * @brief Resets the PIN due to it being too long since last keypress. Shows yellow led, 
 * and plays error sound effect.
 */
void timeoutPIN();

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
 * @brief Checks if enough time has passed since last keypadUpdate to update again.
 * 
 * @return true If enough time has passed.
 * @return false If not enough time has passed.
 */
bool enoughTimeSinceLastKeypadUpdate();



/**
 * @brief Set the values that are read from file by config_handler and are needed by keypad.
 * 
 * @param config Struct holding configuration variables used by keypad and PIN reading.
 * @param keyPins Struct holding the pin numbers for each row and column of the keypad.
 */
//void setKeypadValues(struct KeypadConfig *config, struct KeypadGPIOPins *keyPins, struct Keypad *state);
void setKeypadValues(struct KeypadConfig *config, struct KeypadGPIOPins *keyPins, char **keys);

/**
 * @brief Initializes all arrays used by keypad requiring malloc/calloc.
 */
void initializeKeypad();

/**
 * @brief Frees all arrays used by keypad requiring malloc/calloc.
 */
void cleanupKeypad();



/**
 * @brief Prints a 4x4 array of zeroes and ones represeting the keypad key status. 
 * Ones marking a key in the keypad being currently pressed down.
 */
void printKeyStatus();



#endif // KEYPAD_H