/**
 * @file keypad.c
 * @author Selkamies
 * 
 * @brief Handles the input from a keypad attached to Raspberry Pi 4. 
 * This file contains the logic, all GPIO pin handling by pigpio is in keypad_gpio.c.
 * 
 * @date Created  2023-11-13
 * @date Modified 2023-11-30
 * 
 * @copyright Copyright (c) 2023
 * 
 * TODO: Start getting keypadState.keys from config_handler in setKeypadValues().
 * TODO: Move getCurrentTimeInSeconds() to it's own file? It can be used by leds.c.
 * TODO: Actually check the PINs from a database when it and the code handling it exists.
 */



#include <stdio.h>              // printf()
#include <stdbool.h>
#include <stdlib.h>             // calloc()
#include <string.h>             // strcmp()

#include "keypad.h"
#include "gpio_functions.h"     // All GPIO manipulation is here, currently using pigpio.
#include "leds.h"               // For turning leds on or off.
#include "sounds.h"



#pragma region Globals

/* GPIO pin numbers of the keypad row and column pins. */
struct KeypadGPIOPins keypadPins;

/* Keypad configuration values like KEYPRESS_TIMEOUT. */
struct KeypadConfig keypadConfig;

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

/* Current state of the pin the user is trying to input, if any. */
struct CurrentPinInput currentPinState;

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

/* Keys on the keypad and their state when previously checked. */
struct Keypad keypadState;

#pragma endregion // Globals

#pragma region FunctionDeclarations

/**
 * @brief Loops through the keypad and updates a struct holding data about it's current status,
 * like whether any keys are pressed, what key is pressed, etc.
 */
static void updateKeypadStatus();

/**
 * @brief Stores the pressed key to the current PIN under input.
 * 
 * @param key Key to save.
 */
static void storeKeyPress(char key);

/**
 * @brief Resets the currently input PIN.
 */
static void clearPIN();

/**
 * @brief Checks the full PIN for validity. Currently mock checks.
 * TODO: Check database for existing person that has the passed PIN.
 * 
 * @param pin_input The PIN to check.
 * @return true If the PIN matches.
 * @return false If the PIN doesn't have a match.
 */
static bool validPIN(char *pin_input);

/**
 * @brief Checks if it has been too long since the last keypress.
 * @return true If it has been too long.
 * @return false If it hasn't been too long.
 */
static bool tooLongSinceLastKeypress();

/**
 * @brief Starts the timer counting the time since last keypress by recordin current time.
 * Called at every keypad key press.
 */
static void startTimeoutTimer();

/**
 * @brief Stops the timer counting the time since last keypress.
 * Called at initialization, when the timer reaches defined time limit, or when PIN is rejected or accepted.
 */
static void stopTimeoutTimer();

/**
 * @brief Resets the PIN due to it being too long since last keypress. Shows yellow led, 
 * and plays error sound effect.
 */
static void timeoutPIN();

/**
 * @brief Checks if enough time has passed since last keypadUpdate to update again.
 * 
 * @return true If enough time has passed.
 * @return false If not enough time has passed.
 */
static bool enoughTimeSinceLastKeypadUpdate();

#pragma endregion // FunctionDeclarations



void updateKeypad()
{
    if (enoughTimeSinceLastKeypadUpdate())
    {
        updateKeypadStatus();

        if (keypadState.exactlyOneKeyPressed && keypadState.noKeysPressedPreviously)
        {
            storeKeyPress(keypadState.keyPressed);
        }

        keypadState.noKeysPressedPreviously = !keypadState.anyKeysPressed;

        if (tooLongSinceLastKeypress())
        {
            timeoutPIN();
        }

        keypadState.lastUpdateTime = getCurrentTimeInSeconds();
    }
} 

static void updateKeypadStatus()
{
    // Number of keys that are pressed/down this update.
    int keysNowPressedCount = 0;

    for (int row = 0; row < keypadConfig.KEYPAD_ROWS; row++)
    {
        // Disable the current row to check if any key in this row is pressed.
        turnGPIOPinOff(keypadPins.keypad_rows[row]);

        // Check every column pin to see if a key in this row is pressed.
        for (int column = 0; column < keypadConfig.KEYPAD_COLUMNS; column++)
        {
            // Row off and column on means that they key in the intersection is pressed.
            bool keyNowPressed = isGPIOPinOn(keypadPins.keypad_columns[column]);

            if (keyNowPressed)
            {
                keypadState.keyPressed = keypadState.keys[row][column];
                keypadState.anyKeysPressed = true;

                keysNowPressedCount++;

                // More than one key is pressed down at the same time, we don't accept ambigious input.
                if (keysNowPressedCount > 1)
                {
                    keypadState.exactlyOneKeyPressed = false;
                    // This isn't necessary, but we should probably do it just in case.
                    keypadState.keyPressed = EMPTY_KEY;

                    return;
                }
            }

            keypadState.keysPressedPreviously[row][column] = keyNowPressed;
        }

        // Enable the current row to check the next one.
        turnGPIOPinOn(keypadPins.keypad_rows[row]);
    }

    if (keysNowPressedCount == 1)
    {
        keypadState.exactlyOneKeyPressed = true;
    }

    else if (keysNowPressedCount == 0)
    {
        keypadState.keyPressed = EMPTY_KEY;
        keypadState.anyKeysPressed = false;
        keypadState.exactlyOneKeyPressed = false; 
    }
}



static void storeKeyPress(char key)
{
    // If the there's a led still on, turn it off when we get the first input.
    turnLedsOff();

    // Save the pressed key and record the time.
    currentPinState.keyPresses[currentPinState.nextPressIndex] = key;
    startTimeoutTimer();

    printf("Character %d / %d of PIN entered. Character %c. ", currentPinState.nextPressIndex + 1, keypadConfig.MAX_PIN_LENGTH, key);
    printf("PIN: %s\n", currentPinState.keyPresses);

    currentPinState.nextPressIndex++;

    // If the next key press index would be at the pin length, we just received the last key for the PIN (by length).
    // Check the pin for validity and clear the saved pin.
    if (currentPinState.nextPressIndex >= keypadConfig.MAX_PIN_LENGTH)
    {
        if (validPIN(currentPinState.keyPresses))
        {
            // TODO: Do database things.
            printf("\nCORRECT PIN! - %s \n\n", currentPinState.keyPresses);

            turnLedOn(false, true, false);      // Green light.
            playSound(SOUND_BEEP_SUCCESS);
        }

        else
        {
            printf("\nPIN REJECTED! - %s \n\n", currentPinState.keyPresses);

            turnLedOn(true, false, false);      // Red light.
            playSound(SOUND_BEEP_ERROR);
        }

        clearPIN();
        stopTimeoutTimer();
    }

    else
    {
        playSound(SOUND_BEEP_NORMAL);
    }
}

static void clearPIN()
{
    //currentPinState.lastKeyPressTime = EMPTY_TIMESTAMP;
    currentPinState.nextPressIndex = 0;

    for (int pinIndex = 0; pinIndex < keypadConfig.MAX_PIN_LENGTH; pinIndex++)
    {
        currentPinState.keyPresses[pinIndex] = EMPTY_KEY;
    }
}

static bool validPIN(char *pin_input)
{
    // TODO: Check if we find user with this PIN from database.
    if (strcmp(pin_input, "123A") == 0)
    {

        return true;
    }

    else
    {
        return false;
    }
}

static void startTimeoutTimer()
{
    currentPinState.lastPressTimerOn = true;
    currentPinState.lastKeyPressTime = getCurrentTimeInSeconds();
}

static void stopTimeoutTimer()
{
    currentPinState.lastPressTimerOn = false;
    //currentPinState.lastKeyPressTime = EMPTY_TIMESTAMP;
}

static void timeoutPIN()
{
    // Yellow led.
    turnLedOn(true, true, false);
    clearPIN();
    stopTimeoutTimer();

    playSound(SOUND_BEEP_ERROR);

    printf("\nToo long since last keypress, resetting PIN.\n\n");
}

static bool tooLongSinceLastKeypress()
{
    if (currentPinState.lastPressTimerOn)
    {
        double currentTimeInSeconds = getCurrentTimeInSeconds();
        double timeSinceLastKeyPress = currentTimeInSeconds - currentPinState.lastKeyPressTime;

        if (timeSinceLastKeyPress >= keypadConfig.KEYPRESS_TIMEOUT)
        {
            return true;
        }

        else
        {
            return false;
        }
    }

    else
    {
        return false;
    }
}

double getCurrentTimeInSeconds()
{
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);

    // Adds up the amount of seconds (whole number) and amount of nanoseconds converted to seconds (double).
    return currentTime.tv_sec + (currentTime.tv_nsec / 1e9);
}

static bool enoughTimeSinceLastKeypadUpdate() 
{
    double currentTimeInSeconds = getCurrentTimeInSeconds();
    double timeSinceLastKeypadUpdate = currentTimeInSeconds - keypadState.lastUpdateTime;

    if (timeSinceLastKeypadUpdate >= keypadState.updateInterval)
    {
        return true;
    }

    else
    {
        return false;
    }
}



void setKeypadValues(struct KeypadConfig *config, struct KeypadGPIOPins *keyPins, char **keys)
{
    keypadConfig = *config;
    keypadPins = *keyPins;    
    //keypadState = *state;

    initializeKeypadGPIOPins(&keypadPins, &keypadConfig);
}



void initializeKeypad()
{
    printf("Initializing keypad.\n");

    currentPinState.nextPressIndex = 0;
    stopTimeoutTimer();

    // Initializes the array holding the characters used in the current PIN.
    currentPinState.keyPresses = calloc(keypadConfig.MAX_PIN_LENGTH, sizeof(char));

    if (currentPinState.keyPresses == NULL) 
    {
        printf("\nERROR: Memory allocation failure in keypad.c, initializeKeyboard(), currentPinState.keyPresses!\n");
    }

    for (int index = 0; index < keypadConfig.MAX_PIN_LENGTH; index++)
    {
        currentPinState.keyPresses[index] = EMPTY_KEY;
    }



    keypadState.keys = malloc(keypadConfig.KEYPAD_ROWS * sizeof(char*));
    keypadState.keysPressedPreviously = calloc(keypadConfig.KEYPAD_ROWS, sizeof(bool*));
    keypadState.noKeysPressedPreviously = false;
    keypadState.keyPressed = EMPTY_KEY;
    keypadState.exactlyOneKeyPressed = false;
    keypadState.anyKeysPressed = false;
    keypadState.lastUpdateTime = getCurrentTimeInSeconds();
    // TODO: Read this from file.
    keypadState.updateInterval = 0.1;

    if (keypadState.keys == NULL) 
    {
        printf("\nERROR: Memory allocation failure in keypad.c, initializeKeyboard(), keypadState.keys!\n");
    }

    if (keypadState.keysPressedPreviously == NULL) 
    {
        printf("\nERROR: Memory allocation failure in keypad.c, initializeKeyboard(), keypadState.keysPressedPreviously!\n");
    }

    for (int index = 0; index < keypadConfig.KEYPAD_ROWS; index++) 
    {
        keypadState.keys[index] = malloc(keypadConfig.KEYPAD_COLUMNS * sizeof(char));

        if (keypadState.keys[index] == NULL) 
        {
            printf("\nERROR: Memory allocation failure in keypad.c, initializeKeyboard(), keypadState.keys[%d]!\n", index);
        }

        // Initializes all pressed states to 0 (false).
        keypadState.keysPressedPreviously[index] = calloc(keypadConfig.KEYPAD_COLUMNS, sizeof(bool));

        if (keypadState.keysPressedPreviously[index] == NULL) 
        {
            printf("\nERROR: Memory allocation failure in keypad.c, initializeKeyboard(), keypadState.keysPressedPreviously[%d]!\n", index);
        }
    }

    // TODO: Read these from file, this is temporary.
    char keyArray[4][4] =
    {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };

    // Copy the values to keypadState.keys.
    for (int row = 0; row < keypadConfig.KEYPAD_ROWS; row++) 
    {
        for (int column = 0; column < keypadConfig.KEYPAD_COLUMNS; column++) 
        {
            keypadState.keys[row][column] = keyArray[row][column];
        }
    }

    turnLedsOff();
}

void cleanupKeypad()
{
    turnLedsOff();

    free(currentPinState.keyPresses);
    currentPinState.keyPresses = NULL;

    for (int index = 0; index < keypadConfig.KEYPAD_ROWS; index++)
    {
        free(keypadState.keys[index]);
        free(keypadState.keysPressedPreviously[index]);
        keypadState.keys[index] = NULL;
        keypadState.keysPressedPreviously[index] = NULL;
    }

    free(keypadState.keys);
    free(keypadState.keysPressedPreviously);
    keypadState.keys = NULL;
    keypadState.keysPressedPreviously = NULL;

    cleanupKeypadGPIOPins(&keypadPins, &keypadConfig);
}



void printKeyStatus()
{
    printf("\nKeys:\n");

    for (int row = 0; row < keypadConfig.KEYPAD_ROWS; row++)
    {
        printf("%d %d %d %d\n", keypadState.keysPressedPreviously[row][0], keypadState.keysPressedPreviously[row][1], 
                                keypadState.keysPressedPreviously[row][2], keypadState.keysPressedPreviously[row][3]);
    }
}