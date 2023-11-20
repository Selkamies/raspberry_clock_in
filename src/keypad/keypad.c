/**
 * @file keypad.c
 * @author Selkamies
 * 
 * @brief Handles the input from a keypad attached to Raspberry Pi 4. 
 * This file contains the logic, all GPIO pin handling by pigpio is in keypad_gpio.c.
 * 
 * @date Created  2023-11-13
 * @date Modified 2023-11-20
 * 
 * @copyright Copyright (c) 2023
 * 
 * TODO: Take keypad input on release instead of press?
 * TODO: Prevent accepting multiple keypresses per check?
 * TODO: Play sound with correct/incorrect pin.
 * TODO: Allow PINs of different lengths, some key in the keypad checks the pin.
 * TODO: Instead of clearing the PIN inputs, just keep last X keys and keep checking until success or timeout?
 * TODO: PINs of variable lengths. Starting from some minimum length check every key press? 
 *       What is max length? Probably requires calloc()?
 * TODO: Actually check the PINs from a database when it and the code handling it exists.
 */



#include <stdio.h>
#include <stdlib.h>         // calloc()
#include <stdbool.h>
#include <time.h>           // time_t and time.
#include <string.h>         // strcmp()

#include "keypad.h"
#include "config.h"         // Config values in struct.
#include "keypad_gpio.h"    // All GPIO manipulation is here, currently using pigpio.
#include "leds.h"           // For turning leds on or off.



struct KeypadGPIOPins keypadPins;

/* Current state of the pin the user is trying to input, if any. */
struct CurrentPinInput currentPinState;

/* Keys on the keypad and their state when previously checked. */
struct Keypad keypadState;



// TODO: Currently multiple keys can be pressed at once.
// TODO: Act on key releases instead?
//void updateKeypad(struct GPIOPins *gpioPins)
void updateKeypad()
{
    // The key that was pressed, like "1" or "#".
    char pressedKey = EMPTY_KEY;

    for (int row = 0; row < config.KEYPAD_ROWS; row++)
    {
        // Disable the current row to check if any key in this row is pressed.
        turnKeypadRowOff(keypadPins.keypad_rows[row]);

        // Check every column pin to see if a key in this row is pressed.
        for (int column = 0; column < config.KEYPAD_COLUMNS; column++)
        {
            // Row off and column off means the key in the intersection is pressed.
            bool keyNowPressed = isKeypadColumnOff(keypadPins.keypad_columns[column]);

            // Key is pressed, but was not previously.
            if (keyNowPressed && !keypadState.keysPressedPreviously[row][column])
            {
                pressedKey = keypadState.keys[row][column];
                keypadState.keysPressedPreviously[row][column] = true;
                storeKeyPress(pressedKey);
            }

            // Key was pressed previously, but is not now.
            else if (!keyNowPressed && keypadState.keysPressedPreviously[row][column])
            {
                keypadState.keysPressedPreviously[row][column] = false;
            }
        }

        // Enable the current row to check the next one.
        turnKeypadRowOn(keypadPins.keypad_rows[row]);
    }

    tooLongSinceLastKeypress();
} 



void storeKeyPress(char key)
{
    // Just in case
    turnLedsOff();

    // Save the pressed key and record the time.
    currentPinState.keyPresses[currentPinState.nextPressIndex] = key;
    startTimeoutTimer();

    printf("Index %d of PIN entered. Character %c.\n", currentPinState.nextPressIndex, key);

    currentPinState.nextPressIndex++;

    // If the next key press index would be at the pin length, 
    // we just received the last key for the PIN (by length).
    // Check the pin for validity and clear the saved pin.
    if (currentPinState.nextPressIndex >= config.MAX_PIN_LENGTH)
    {
        if (checkPIN(currentPinState.keyPresses))
        {
            // TODO: Do database things.
            printf("\nCORRECT PIN! - %s \n", currentPinState.keyPresses);
            turnLedOn(false, true, false);
        }

        else
        {
            printf("\nPIN REJECTED! - %s \n", currentPinState.keyPresses);
            turnLedOn(true, false, false);
        }

        clearPIN();
        resetTimeoutTimer();
    }
}

void clearPIN()
{
    currentPinState.lastKeyPressTime = EMPTY_TIMESTAMP;
    currentPinState.nextPressIndex = 0;

    for (int pinIndex = 0; pinIndex < config.MAX_PIN_LENGTH; pinIndex++)
    {
        currentPinState.keyPresses[pinIndex] = EMPTY_KEY;
    }
}

bool checkPIN(char *pin_input)
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
    currentPinState.lastKeyPressTime = time(NULL);
}

void resetTimeoutTimer()
{
    currentPinState.lastPressTimerOn = false;
    currentPinState.lastKeyPressTime = EMPTY_TIMESTAMP;
}

bool tooLongSinceLastKeypress()
{
    if (currentPinState.lastPressTimerOn)
    {
        time_t currentTime = time(NULL);
        double time_since_last_press = difftime(currentTime, currentPinState.lastKeyPressTime);

        if (currentPinState.lastKeyPressTime != 0 && time_since_last_press >= config.KEYPRESS_TIMEOUT)
        {
            // Yellow led.
            turnLedOn(true, true, false);
            clearPIN();
            resetTimeoutTimer();

            printf("Too long since last keypress, resetting PIN.\n");

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



void printKeyStatus()
{
    printf("\nKeys:\n");

    for (int row = 0; row < config.KEYPAD_ROWS; row++)
    {
        printf("%d %d %d %d\n", keypadState.keysPressedPreviously[row][0], keypadState.keysPressedPreviously[row][1], 
                                keypadState.keysPressedPreviously[row][2], keypadState.keysPressedPreviously[row][3]);
    }
}



void setKeypadValues(struct KeypadGPIOPins *keyPins)
{
    keypadPins = *keyPins;

    initializeGPIOPins(&keypadPins);
}



void initializeKeypad()
{
    printf("Initializing keypad.\n");

    currentPinState.nextPressIndex = 0;
    resetTimeoutTimer();

    // Initializes the array holding the characters used in the current PIN.
    currentPinState.keyPresses = calloc(config.MAX_PIN_LENGTH, sizeof(char));

    if (currentPinState.keyPresses == NULL) 
    {
        printf("\nERROR: Memory allocation failure in keypad.c, initializeKeyboard(), currentPinState.keyPresses!\n");
    }

    for (int index = 0; index < config.MAX_PIN_LENGTH; index++)
    {
        currentPinState.keyPresses[index] = EMPTY_KEY;
    }



    keypadState.keys = malloc(config.KEYPAD_ROWS * sizeof(char*));
    keypadState.keysPressedPreviously = calloc(config.KEYPAD_ROWS, sizeof(bool*));

    if (keypadState.keys == NULL) 
    {
        printf("\nERROR: Memory allocation failure in keypad.c, initializeKeyboard(), keypadState.keys!\n");
    }

    if (keypadState.keysPressedPreviously == NULL) 
    {
        printf("\nERROR: Memory allocation failure in keypad.c, initializeKeyboard(), keypadState.keysPressedPreviously!\n");
    }

    for (int index = 0; index < config.KEYPAD_ROWS; index++) 
    {
        keypadState.keys[index] = malloc(config.KEYPAD_COLUMNS * sizeof(char));

        if (keypadState.keys[index] == NULL) 
        {
            printf("\nERROR: Memory allocation failure in keypad.c, initializeKeyboard(), keypadState.keys[%d]!\n", index);
        }

        // Initializes all pressed states to 0 (false).
        keypadState.keysPressedPreviously[index] = calloc(config.KEYPAD_COLUMNS, sizeof(bool));

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
    for (int row = 0; row < config.KEYPAD_ROWS; row++) 
    {
        for (int column = 0; column < config.KEYPAD_COLUMNS; column++) 
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

    for (int index = 0; index < config.KEYPAD_ROWS; index++)
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

    cleanupGPIOPins(&keypadPins);
}