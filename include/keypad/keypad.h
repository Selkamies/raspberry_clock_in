#ifndef KEYPAD_H
#define KEYPAD_H



#include <stdbool.h>
#include <time.h>

//#include "pins.h"



/** @brief Maximum length of the pin code. */
#define MAX_PIN_LENGTH 4
/** @brief Time in seconds after the last keypress, after which the program stops waiting and resets the pin. */
#define KEYPRESS_TIMEOUT 10
/** @brief Number of rows in the keypad. */
#define KEYPAD_ROWS 4
/** @brief Number of columns in the keypad. */
#define KEYPAD_COLUMNS 4

/** @brief Default value used for empty characters when no character is received or set. */
#define EMPTY_KEY '\0'
/** @brief Default value used for timestamps when no time is recorded. */
#define EMPTY_TIMESTAMP 0



// Forward declaration.
struct GPIOPins;

/**
 * @brief Struct holding data about the pin code the user is currently trying to input, if any.
 */
struct CurrentPinInput
{
    /** @brief Index for the next free char in the array holding the pin code being entered. */
    int nextPressIndex;
    /** @brief Time since the last key was pressed. */
    time_t lastKeyPressTime;
    /** @brief Array holding the pin code being entered. */
    char keyPresses[MAX_PIN_LENGTH];
};

/**
 * @brief Struct holding the keys on the keypad and the previous state of the keypad keys.
 */
struct KeyPad
{
    /** @brief Holds the keys in the keypad, in the corresponding positions. */
    char keys[KEYPAD_ROWS][KEYPAD_COLUMNS];
    /** @brief State of keys (pressed or not) during previous check. 
     * Used to check for changes, so that we only register button press once. */
    bool keysPressedPreviously[KEYPAD_ROWS][KEYPAD_COLUMNS];
};



/**
 * @brief Checks if any key in the keypad was just pressed, and handles it accordingly.
 * 
 * @param gpioPins Struct holding the GPIO pin numbers.
 */
char checkKeyPress(struct GPIOPins *gpioPins);

/**
 * @brief Stores the pressed key to the current pin code under input.
 * 
 * @param key Key to save.
 */
void storeKeyPress(char key);

/**
 * @brief Resets the currently input pin code.
 */
void clearKeys();

bool checkPin();

/**
 * @brief Checks if it has been too long since the last keypress.
 * 
 * @return true If time since last keypress exceeded wait time.
 * @return false If time since last keypress is under wait time.
 */
bool keypressTimeOut();

void printKeyStatus();



#endif // KEYPAD_H