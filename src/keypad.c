/**
 * @file keypad.c
 * @author Selkamies
 * 
 * @brief Handles the input from a keypad attached to Raspberry Pi 4. 
 * This file contains the logic, all GPIO pin handling by pigpio is in keypad_gpio.c.
 * 
 * @date Created  2023-11-13
 * @date Modified 2023-12-07
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
#include "gpio_functions.h"     // turnGPIOPinOff(), turnGPIOPinOn(), isGPIOPinOn().
#include "leds.h"               // turnLEDOn(), turnLEDsOff().
#include "sounds.h"             // playSound().
#include "timer.h"              // getCurrentTimeInSeconds().



#pragma region Globals

/** @brief Default value used for empty characters when no character is received or set. */
#define EMPTY_KEY '\0'
/** @brief Default value used for timestamps when no time is recorded. */
#define EMPTY_TIMESTAMP 0



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
    /** @brief Time when the last key was pressed. */
    double lastKeyPressTime;
    /** @brief Array holding the PIN being entered. */
    char *keyPresses;
};



/* GPIO pin numbers of the keypad row and column pins. */
struct KeypadGPIOPins keypadPins;

/* Keypad configuration values like KEYPRESS_TIMEOUT. */
struct KeypadConfig keypadConfig;

/* Keys on the keypad and their state when previously checked. */
struct Keypad keypadState;

/* Current state of the pin the user is trying to input, if any. */
struct CurrentPinInput currentPinState;

#pragma endregion // Globals

#pragma region FunctionDeclarations

/**
 * @brief Loops through the keypad and updates a struct holding data about it's current status,
 * like whether any keys are pressed, what key is pressed, etc.
 */
static void updateKeypadStatus();

/**
 * @brief Loops through the keypad keys and checks if they are pressed or not. Counts and returns
 * the number of keys currently pressed. If exactly one key is pressed, notes it in keypadState.keyPressed.
 * 
 * @return int Number of keys currently pressed on the keypad. 0-2.
 * If the number gets above 1, we break the loop early and don't bother checking for the rest of the keys.
 */
static int loopThroughKeys();

/**
 * @brief Stores the pressed key to the current PIN under input.
 * 
 * @param key Key to save.
 */
static void storeKeyPress(struct LEDConfig *LEDConfigData, const struct SoundsConfig *soundsConfig, const char key);

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
static bool validPIN(const char *pin_input);

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
static void timeoutPIN(struct LEDConfig *LEDConfigData, const struct SoundsConfig *soundsConfig);

/**
 * @brief Checks if enough time has passed since last keypadUpdate to update again.
 * 
 * @return true If enough time has passed.
 * @return false If not enough time has passed.
 */
static bool enoughTimeSinceLastKeypadUpdate();

#pragma endregion // FunctionDeclarations



void updateKeypad(struct LEDConfig *LEDConfigData, struct SoundsConfig *soundsConfig)
{
    if (enoughTimeSinceLastKeypadUpdate())
    {
        updateKeypadStatus();

        if (keypadState.exactlyOneKeyPressed && keypadState.noKeysPressedPreviously)
        {
            storeKeyPress(LEDConfigData, soundsConfig, keypadState.keyPressed);
        }

        keypadState.noKeysPressedPreviously = !keypadState.anyKeysPressed;

        if (tooLongSinceLastKeypress())
        {
            timeoutPIN(LEDConfigData, soundsConfig);
        }

        keypadState.lastUpdateTime = getCurrentTimeInSeconds();
    }
} 

static void updateKeypadStatus()
{
    // Number of keys that are pressed/down this update.
    int keysNowPressedCount = loopThroughKeys();

    if (keysNowPressedCount == 0)
    {
        //printf("No keys pressed.\n");
        keypadState.keyPressed = EMPTY_KEY;
        keypadState.anyKeysPressed = false;
        keypadState.exactlyOneKeyPressed = false; 
    }

    else if (keysNowPressedCount == 1)
    {
        //printf("Exactly one key pressed.\n");
        keypadState.anyKeysPressed = true;
        keypadState.exactlyOneKeyPressed = true;
    }

    else if (keysNowPressedCount > 1)
    {
        //printf("Too many keys pressed.\n");
        keypadState.anyKeysPressed = true;
        keypadState.exactlyOneKeyPressed = false;
        // This isn't necessary, but we should probably do it just in case.
        keypadState.keyPressed = EMPTY_KEY;
    }
}

static int loopThroughKeys()
{
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
                keysNowPressedCount++;

                // More than one key is pressed down at the same time, we don't accept ambigious input.
                if (keysNowPressedCount > 1)
                {
                    // TODO: Is this necessary?
                    turnGPIOPinOn(keypadPins.keypad_rows[row]);

                    return keysNowPressedCount;
                }
            }

            keypadState.keysPressedPreviously[row][column] = keyNowPressed;
        }

        // Enable the current row to check the next one.
        turnGPIOPinOn(keypadPins.keypad_rows[row]);
    }

    return keysNowPressedCount;
}



static void storeKeyPress(struct LEDConfig *LEDConfigData, const struct SoundsConfig *soundsConfig, const char key)
{
    // If the there's a led still on, turn it off when we get the first input.
    turnLEDsOff(LEDConfigData);

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

            turnLEDOn(LEDConfigData, false, true, false);      // Green light.
            playSound(soundsConfig, SOUND_BEEP_SUCCESS);
        }

        else
        {
            printf("\nPIN REJECTED! - %s \n\n", currentPinState.keyPresses);

            turnLEDOn(LEDConfigData, true, false, false);      // Red light.
            playSound(soundsConfig, SOUND_BEEP_ERROR);
        }

        clearPIN();
        stopTimeoutTimer();
    }

    else
    {
        playSound(soundsConfig, SOUND_BEEP_NORMAL);
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

static bool validPIN(const char *pin_input)
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

static void timeoutPIN(struct LEDConfig *LEDConfigData, const struct SoundsConfig *soundsConfig)
{
    // Yellow led.
    turnLEDOn(LEDConfigData, true, true, false);
    clearPIN();
    stopTimeoutTimer();

    playSound(soundsConfig, SOUND_BEEP_ERROR);

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

static bool enoughTimeSinceLastKeypadUpdate() 
{
    double timeSinceLastKeypadUpdate = getCurrentTimeInSeconds() - keypadState.lastUpdateTime;

    if (timeSinceLastKeypadUpdate >= keypadConfig.UPDATE_INTERVAL_SECONDS)
    {
        return true;
    }

    else
    {
        return false;
    }
}



void setKeypadValues(struct KeypadConfig *config, struct KeypadGPIOPins *keyPins, char **keypadKeys)
{
    keypadConfig = *config;
    keypadPins = *keyPins;    
    keypadState.keys = keypadKeys;

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



    keypadState.keysPressedPreviously = calloc(keypadConfig.KEYPAD_ROWS, sizeof(bool*));
    keypadState.noKeysPressedPreviously = false;
    keypadState.keyPressed = EMPTY_KEY;
    keypadState.exactlyOneKeyPressed = false;
    keypadState.anyKeysPressed = false;
    keypadState.lastUpdateTime = getCurrentTimeInSeconds();

    if (keypadState.keysPressedPreviously == NULL) 
    {
        printf("\nERROR: Memory allocation failure in keypad.c, initializeKeyboard(), keypadState.keysPressedPreviously!\n");
    }

    for (int index = 0; index < keypadConfig.KEYPAD_ROWS; index++) 
    {
        // Initializes all pressed states to 0 (false).
        keypadState.keysPressedPreviously[index] = calloc(keypadConfig.KEYPAD_COLUMNS, sizeof(bool));

        if (keypadState.keysPressedPreviously[index] == NULL) 
        {
            printf("\nERROR: Memory allocation failure in keypad.c, initializeKeyboard(), keypadState.keysPressedPreviously[%d]!\n", index);
        }
    }

    /* if (keypadState.keys == NULL) 
    {
        printf("\nERROR: Memory allocation failure in keypad.c, initializeKeyboard(), keypadState.keys!\n");
    } */

    //turnLEDsOff();
}

void cleanupKeypad(struct LEDConfig *LEDConfigData)
{
    turnLEDsOff(LEDConfigData);

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