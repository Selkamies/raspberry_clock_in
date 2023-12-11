/**
 * @file keypad.c
 * @author Selkamies
 * 
 * @brief Handles the input from a keypad attached to Raspberry Pi 4. 
 * This file contains the logic, all GPIO pin handling by pigpio is in keypad_gpio.c.
 * 
 * @date Created  2023-11-13
 * @date Modified 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
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
#include "database.h"           // 

#include "config_data.h"        // struct ConfigData.
#include "keypad_config.h"      // struct KeypadConfig, struct KeypadState, struct PINState.



/** @brief Default value used for empty characters when no character is received or set. */
#define EMPTY_KEY '\0'
/** @brief Default value used for timestamps when no time is recorded. */
#define EMPTY_TIMESTAMP 0



#pragma region FunctionDeclarations

/**
 * @brief Loops through the keypad and updates a struct holding data about it's current status,
 * like whether any keys are pressed, what key is pressed, etc.
 * 
 * @param keypadConfig Struct holding configuration variables used by keypad and PIN reading.
 */
static void updateKeypadStatus(struct KeypadConfig *keypadConfig);

/**
 * @brief Loops through the keypad keys and checks if they are pressed or not. Counts and returns
 * the number of keys currently pressed. If exactly one key is pressed, notes it in keypadState.keyPressed.
 * 
 * @param keypadConfig Struct holding configuration variables used by keypad and PIN reading.
 * 
 * @return int Number of keys currently pressed on the keypad. 0-2.
 * If the number gets above 1, we break the loop early and don't bother checking for the rest of the keys.
 */
static int loopThroughKeys(struct KeypadConfig *keypadConfig);

/**
 * @brief Stores the pressed key to the current PIN under input.
 * 
 * @param configData Struct holding data about basically all variables used by the program.
 * @param key Key to save.
 */
static void storeKeyPress(struct ConfigData *configData, const char key);

/**
 * @brief Resets the currently input PIN.
 * 
 * @param keypadConfig Struct holding configuration variables used by keypad and PIN reading.
 */
static void clearPIN(struct KeypadConfig *keypadConfig);

/**
 * @brief Checks the full PIN for validity. Currently mock checks.
 * TODO: Check database for existing person that has the passed PIN.
 * 
 * @param database Database we're using.
 * @param pin_input The PIN to check.
 * 
 * @return true If the PIN matches.
 * @return false If the PIN doesn't have a match.
 */
static bool validPIN(sqlite3 **database, const char *pin_input);

/**
 * @brief Checks if it has been too long since the last keypress.
 * 
 * @param keypadConfig Struct holding configuration variables used by keypad and PIN reading.
 * 
 * @return true If it has been too long.
 * @return false If it hasn't been too long.
 */
static bool tooLongSinceLastKeypress(const struct KeypadConfig *keypadConfig);

/**
 * @brief Starts the timer counting the time since last keypress by recordin current time.
 * Called at every keypad key press.
 * 
 * @param currentPINState Struct holding data about the PIN the user is currently trying to input, if any.
 */
static void startTimeoutTimer(struct PINState *currentPINState);

/**
 * @brief Stops the timer counting the time since last keypress.
 * Called at initialization, when the timer reaches defined time limit, or when PIN is rejected or accepted.
 * 
 * @param currentPINState Struct holding data about the PIN the user is currently trying to input, if any.
 */
static void stopTimeoutTimer(struct PINState *currentPINState);

/**
 * @brief Resets the PIN due to it being too long since last keypress. Shows yellow led, 
 * and plays error sound effect.
 * 
 * @param configData Struct holding data about basically all variables used by the program.
 */
static void timeoutPIN(struct ConfigData *configData);

/**
 * @brief Checks if enough time has passed since last keypadUpdate to update again.
 * 
 * @param lastUpdateTime Time of the last update of keypad state.
 * @param updateInterval How often the keypad state should be updated, in seconds.
 * 
 * @return true If enough time has passed.
 * @return false If not enough time has passed.
 */
static bool enoughTimeSinceLastKeypadUpdate(const double lastUpdateTime, const double updateInterval);

#pragma endregion // FunctionDeclarations



void updateKeypad(struct ConfigData *configData)
{
    // For readability.
    struct KeypadConfig *keypadConfig = &configData->keypadConfig;
    struct KeypadState *keypadState = &keypadConfig->keypadState;

    if (enoughTimeSinceLastKeypadUpdate(keypadState->lastUpdateTime, keypadConfig->UPDATE_INTERVAL_SECONDS))
    {
        updateKeypadStatus(keypadConfig);

        if (keypadState->exactlyOneKeyPressed && keypadState->noKeysPressedPreviously)
        {
            storeKeyPress(configData, keypadState->keyPressed);
        }

        keypadState->noKeysPressedPreviously = !keypadState->anyKeysPressed;

        if (tooLongSinceLastKeypress(keypadConfig))
        {
            timeoutPIN(configData);
        }

        keypadState->lastUpdateTime = getCurrentTimeInSeconds();
    }
} 

static void updateKeypadStatus(struct KeypadConfig *keypadConfig)
{
    // For readability.
    struct KeypadState *keypadState = &keypadConfig->keypadState;

    // Number of keys that are pressed/down this update.
    int keysNowPressedCount = loopThroughKeys(keypadConfig);

    if (keysNowPressedCount == 0)
    {
        //printf("No keys pressed.\n");
        keypadState->keyPressed = EMPTY_KEY;
        keypadState->anyKeysPressed = false;
        keypadState->exactlyOneKeyPressed = false; 
    }

    else if (keysNowPressedCount == 1)
    {
        //printf("Exactly one key pressed.\n");
        keypadState->anyKeysPressed = true;
        keypadState->exactlyOneKeyPressed = true;
    }

    else if (keysNowPressedCount > 1)
    {
        //printf("Too many keys pressed.\n");
        keypadState->anyKeysPressed = true;
        keypadState->exactlyOneKeyPressed = false;
        // This isn't necessary, but we should probably do it just in case.
        keypadState->keyPressed = EMPTY_KEY;
    }
}

static int loopThroughKeys(struct KeypadConfig *keypadConfig)
{
    int keysNowPressedCount = 0;

    for (int row = 0; row < keypadConfig->KEYPAD_ROWS; row++)
    {
        // Disable the current row to check if any key in this row is pressed.
        turnGPIOPinOff(keypadConfig->pins.keypad_rows[row]);

        // Check every column pin to see if a key in this row is pressed.
        for (int column = 0; column < keypadConfig->KEYPAD_COLUMNS; column++)
        {
            // Row off and column on means that they key in the intersection is pressed.
            bool keyNowPressed = isGPIOPinOn(keypadConfig->pins.keypad_columns[column]);

            if (keyNowPressed)
            {
                keypadConfig->keypadState.keyPressed = keypadConfig->keypadState.keys[row][column];
                keysNowPressedCount++;

                // More than one key is pressed down at the same time, we don't accept ambigious input.
                if (keysNowPressedCount > 1)
                {
                    // TODO: Is this necessary?
                    turnGPIOPinOn(keypadConfig->pins.keypad_rows[row]);

                    return keysNowPressedCount;
                }
            }

            keypadConfig->keypadState.keysPressedPreviously[row][column] = keyNowPressed;
        }

        // Enable the current row to check the next one.
        turnGPIOPinOn(keypadConfig->pins.keypad_rows[row]);
    }

    return keysNowPressedCount;
}



static void storeKeyPress(struct ConfigData *configData, const char key)
{
    // For readability.
    struct PINState *currentPINState = &configData->keypadConfig.currentPINState;

    // If the there's a led still on, turn it off when we get the first input.
    turnLEDsOff(&configData->LEDConfigData);

    // Save the pressed key and record the time.
    currentPINState->keyPresses[currentPINState->nextPressIndex] = key;
    startTimeoutTimer(&configData->keypadConfig.currentPINState);

    printf("Character %d / %d of PIN entered. Character %c. ", currentPINState->nextPressIndex + 1, 
                                                               configData->keypadConfig.MAX_PIN_LENGTH, key);
    printf("PIN: %s\n", currentPINState->keyPresses);

    currentPINState->nextPressIndex++;

    // If the next key press index would be at the pin length, we just received the last key for the PIN (by length).
    // Check the pin for validity and clear the saved pin.
    if (currentPINState->nextPressIndex >= configData->keypadConfig.MAX_PIN_LENGTH)
    {
        if (validPIN(configData->database, currentPINState->keyPresses))
        {
            // TODO: Do database things.
            printf("\nCORRECT PIN! - %s \n\n", currentPINState->keyPresses);

            turnLEDOn(&configData->LEDConfigData, false, true, false);      // Green light.
            playSound(&configData->soundsConfig, SOUND_BEEP_SUCCESS);
        }

        else
        {
            printf("\nPIN REJECTED! - %s \n\n", currentPINState->keyPresses);

            turnLEDOn(&configData->LEDConfigData, true, false, false);      // Red light.
            playSound(&configData->soundsConfig, SOUND_BEEP_ERROR);
        }

        clearPIN(&configData->keypadConfig);
        stopTimeoutTimer(currentPINState);
    }

    else
    {
        playSound(&configData->soundsConfig, SOUND_BEEP_NORMAL);
    }
}

static void clearPIN(struct KeypadConfig *keypadConfig)
{
    //currentPINState.lastKeyPressTime = EMPTY_TIMESTAMP;
    keypadConfig->currentPINState.nextPressIndex = 0;

    for (int pinIndex = 0; pinIndex < keypadConfig->MAX_PIN_LENGTH; pinIndex++)
    {
        keypadConfig->currentPINState.keyPresses[pinIndex] = EMPTY_KEY;
    }
}

static bool validPIN(sqlite3 **database, const char *pin_input)
{
    // TODO: Check if we find user with this PIN from database.
    /* if (strcmp(pin_input, "123A") == 0)
    {
        return true;
    } */

    int *userIDPointer;

    if (selectUserIDByPIN(database, pin_input, userIDPointer))
    {
        printf("PIN '%s' belongs to user id of '%d'.\n", pin_input, *userIDPointer);

        return true;
    }

    else
    {
        return false;
    }
}

static void startTimeoutTimer(struct PINState *currentPINState)
{
    currentPINState->lastPressTimerOn = true;
    currentPINState->lastKeyPressTime = getCurrentTimeInSeconds();
}

static void stopTimeoutTimer(struct PINState *currentPINState)
{
    currentPINState->lastPressTimerOn = false;
    //currentPINState.lastKeyPressTime = EMPTY_TIMESTAMP;
}

static void timeoutPIN(struct ConfigData *configData)
{
    // Yellow led.
    turnLEDOn(&configData->LEDConfigData, true, true, false);
    clearPIN(&configData->keypadConfig);
    stopTimeoutTimer(&configData->keypadConfig.currentPINState);

    playSound(&configData->soundsConfig, SOUND_BEEP_ERROR);

    printf("\nToo long since last keypress, resetting PIN.\n\n");
}

static bool tooLongSinceLastKeypress(const struct KeypadConfig *keypadConfig)
{
    if (keypadConfig->currentPINState.lastPressTimerOn)
    {
        double currentTimeInSeconds = getCurrentTimeInSeconds();
        double timeSinceLastKeyPress = currentTimeInSeconds - keypadConfig->currentPINState.lastKeyPressTime;

        if (timeSinceLastKeyPress >= keypadConfig->KEYPRESS_TIMEOUT)
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

static bool enoughTimeSinceLastKeypadUpdate(const double lastUpdateTime, const double updateInterval)
{
    double timeSinceLastKeypadUpdate = getCurrentTimeInSeconds() - lastUpdateTime;

    if (timeSinceLastKeypadUpdate >= updateInterval)
    {
        return true;
    }

    else
    {
        return false;
    }
}



void initializeKeypad(struct KeypadConfig *keypadConfig)
{
    initializeKeypadGPIOPins(keypadConfig);

    printf("Initializing keypad.\n");

    //////////////
    // PINState //
    //////////////

    keypadConfig->currentPINState.nextPressIndex = 0;
    stopTimeoutTimer(&keypadConfig->currentPINState);

    // Initializes the array holding the characters used in the current PIN.
    keypadConfig->currentPINState.keyPresses = calloc(keypadConfig->MAX_PIN_LENGTH, sizeof(char));

    if (keypadConfig->currentPINState.keyPresses == NULL) 
    {
        printf("\nERROR: Memory allocation failure in keypad.c, initializeKeyboard(), currentPINState.keyPresses!\n");
    }

    for (int index = 0; index < keypadConfig->MAX_PIN_LENGTH; index++)
    {
        keypadConfig->currentPINState.keyPresses[index] = EMPTY_KEY;
    }

    /////////////////
    // KeypadState //
    /////////////////

    keypadConfig->keypadState.keysPressedPreviously = calloc(keypadConfig->KEYPAD_ROWS, sizeof(bool*));
    keypadConfig->keypadState.noKeysPressedPreviously = false;
    keypadConfig->keypadState.keyPressed = EMPTY_KEY;
    keypadConfig->keypadState.exactlyOneKeyPressed = false;
    keypadConfig->keypadState.anyKeysPressed = false;
    keypadConfig->keypadState.lastUpdateTime = getCurrentTimeInSeconds();

    if (keypadConfig->keypadState.keysPressedPreviously == NULL) 
    {
        printf("\nERROR: Memory allocation failure in keypad.c, initializeKeyboard(), keypadState.keysPressedPreviously!\n");
    }

    for (int index = 0; index < keypadConfig->KEYPAD_ROWS; index++) 
    {
        // Initializes all pressed states to 0 (false).
        keypadConfig->keypadState.keysPressedPreviously[index] = calloc(keypadConfig->KEYPAD_COLUMNS, sizeof(bool));

        if (keypadConfig->keypadState.keysPressedPreviously[index] == NULL) 
        {
            printf("\nERROR: Memory allocation failure in keypad.c, initializeKeyboard(), keypadState.keysPressedPreviously[%d]!\n", index);
        }
    }

    /* if (keypadState.keys == NULL) 
    {
        printf("\nERROR: Memory allocation failure in keypad.c, initializeKeyboard(), keypadState.keys!\n");
    } */
}

void cleanupKeypad(struct KeypadConfig *keypadConfig)
{
    free(keypadConfig->currentPINState.keyPresses);
    keypadConfig->currentPINState.keyPresses = NULL;

    for (int index = 0; index < keypadConfig->KEYPAD_ROWS; index++)
    {
        free(keypadConfig->keypadState.keys[index]);
        free(keypadConfig->keypadState.keysPressedPreviously[index]);
        keypadConfig->keypadState.keys[index] = NULL;
        keypadConfig->keypadState.keysPressedPreviously[index] = NULL;
    }

    free(keypadConfig->keypadState.keys);
    free(keypadConfig->keypadState.keysPressedPreviously);
    keypadConfig->keypadState.keys = NULL;
    keypadConfig->keypadState.keysPressedPreviously = NULL;

    cleanupKeypadGPIOPins(keypadConfig);
}
