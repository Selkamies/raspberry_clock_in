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
 * TODO: Start using the linux timer with time since last keypress.
 * TODO: Take keypad input on release instead of press?
 * TODO: Allow PINs of different lengths, some key in the keypad checks the pin.
 * TODO: Instead of clearing the PIN inputs, just keep last X keys and keep checking until success or timeout?
 * TODO: PINs of variable lengths. Starting from some minimum length check every key press? 
 *       What is max length? Probably requires calloc()?
 * TODO: Actually check the PINs from a database when it and the code handling it exists.
 */



#include <stdio.h>              // printf()
#include <stdlib.h>             // calloc()
#include <string.h>             // strcmp()

#include "keypad.h"
#include "gpio_functions.h"     // All GPIO manipulation is here, currently using pigpio.
#include "leds.h"               // For turning leds on or off.
#include "sounds.h"



/* GPIO pin numbers of the keypad row and column pins. */
static struct KeypadGPIOPins keypadPins;

/* Keypad configuration values like KEYPRESS_TIMEOUT. */
static struct KeypadConfig keypadConfig;

/* Current state of the pin the user is trying to input, if any. */
static struct CurrentPinInput currentPinState;

/* Keys on the keypad and their state when previously checked. */
static struct Keypad keypadState;



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

void updateKeypadStatus()
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



void storeKeyPress(char key)
{
    // TODO: Check if a led is on?
    // If the there's a led still on, turn it off when we get the first input.
    turnLedsOff();

    // Save the pressed key and record the time.
    currentPinState.keyPresses[currentPinState.nextPressIndex] = key;
    startTimeoutTimer();

    printf("Index %d / %d of PIN entered. Character %c.\n", currentPinState.nextPressIndex, keypadConfig.MAX_PIN_LENGTH - 1, key);

    currentPinState.nextPressIndex++;

    // If the next key press index would be at the pin length, we just received the last key for the PIN (by length).
    // Check the pin for validity and clear the saved pin.
    if (currentPinState.nextPressIndex >= keypadConfig.MAX_PIN_LENGTH)
    {
        if (validPIN(currentPinState.keyPresses))
        {
            // TODO: Do database things.
            printf("\nCORRECT PIN! - %s \n\n", currentPinState.keyPresses);

            turnLedOn(false, true, false);
            playSound(SOUND_BEEP_SUCCESS);
        }

        else
        {
            printf("\nPIN REJECTED! - %s \n\n", currentPinState.keyPresses);

            turnLedOn(true, false, false);
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

void clearPIN()
{
    //currentPinState.lastKeyPressTime = EMPTY_TIMESTAMP;
    currentPinState.nextPressIndex = 0;

    for (int pinIndex = 0; pinIndex < keypadConfig.MAX_PIN_LENGTH; pinIndex++)
    {
        currentPinState.keyPresses[pinIndex] = EMPTY_KEY;
    }
}

bool validPIN(char *pin_input)
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

void startTimeoutTimer()
{
    currentPinState.lastPressTimerOn = true;
    currentPinState.lastKeyPressTime = getCurrentTimeInSeconds();
}

void stopTimeoutTimer()
{
    currentPinState.lastPressTimerOn = false;
    //currentPinState.lastKeyPressTime = EMPTY_TIMESTAMP;
}

void timeoutPIN()
{
    // Yellow led.
    turnLedOn(true, true, false);
    clearPIN();
    stopTimeoutTimer();

    playSound(SOUND_BEEP_ERROR);

    printf("\nToo long since last keypress, resetting PIN.\n\n");
}

bool tooLongSinceLastKeypress()
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

bool enoughTimeSinceLastKeypadUpdate() 
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

    //return ((currentTimeInSeconds - keypadState.lastUpdateTime) >= keypadState.updateInterval);
}



//void setKeypadValues(struct KeypadConfig *config, struct KeypadGPIOPins *keyPins, struct Keypad *state)
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