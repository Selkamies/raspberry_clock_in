/**
 * @file keypad.c
 * @author Selkamies
 * 
 * @brief Handles the input from a keypad attached to Raspberry Pi 4. 
 * This file contains the logic, all GPIO pin handling by pigpio is in keypad_gpio.c.
 * 
 * @date Created  2023-11-13
 * @date Modified 2023-11-15
 * 
 * @copyright Copyright (c) 2023
 * 
 * TODO: Prevent accepting multiple keypresses per check?
 * TODO: Instead of clearing the PIN inputs, just keep last X keys and keep checking until success or timeout.
 * TODO: Allow PINs of different lengths, some key in the keypad checks the pin.
 * TODO: PINs of variable lengths. Starting from some minimum length check every key press? 
 *       What is max length? Probably requires calloc()?
 * TODO: Actually check the PINs from a database when it and the code handling it exists.
 * TODO: KEYPAD_ROWS and KEYPAD_COLUMNS are also needed elsewhere (at least pins.h). 
 *       Move them to their own file? What about other defines?
 * TODO: Move the defines to other file(s) and change to extern consts, then read them from file?
 */



#include <stdio.h>
#include <stdlib.h>         // calloc()
#include <stdbool.h>
#include <time.h>           // time_t and time.
#include <string.h>         // strcmp()

#include "config.h"
#include "pins.h"
#include "keypad.h"
#include "keypad_gpio.h"



/* Current state of the pin the user is trying to input, if any. */
struct CurrentPinInput currentPinState;

/* Keys on the keypad and their state when previously checked. */
struct Keypad keypadState;



void checkKeyPress(struct GPIOPins *gpioPins)
{
    char pressedKey = EMPTY_KEY;
    //bool keyWasPressed = false;

    for (int row = 0; row < config.KEYPAD_ROWS; row++)
    {
        // Disable the current row to check if any key in this row is pressed.
        turnKeypadRowOff(gpioPins->keypad_rows[row]);

        // Check every column pin to see if a key in this row is pressed.
        for (int column = 0; column < config.KEYPAD_COLUMNS; column++)
        {
            // Row off and column off means the key in the intersection is pressed.
            bool keyNowPressed = isKeypadColumnOff(gpioPins->keypad_columns[column]);

            // Key is pressed, but was not previously.
            if (keyNowPressed && !keypadState.keysPressedPreviously[row][column])
            {
                pressedKey = keypadState.keys[row][column];
                keypadState.keysPressedPreviously[row][column] = true;

                //printf("Key %c was just pressed.\n", pressedKey);

                storeKeyPress(pressedKey);
                //keyWasPressed = true;
            }

            // Key was pressed previously, but is not now.
            else if (!keyNowPressed && keypadState.keysPressedPreviously[row][column])
            {
                keypadState.keysPressedPreviously[row][column] = false;
                //pressedKey = keypadState.keys[row][column];
                //printf("Key %c was just released.\n", pressedKey);
            }
        }

        // Enable the current row to check the next one.
        turnKeypadRowOn(gpioPins->keypad_rows[row]);
    }

    //return pressedKey;
}

void storeKeyPress(char key)
{
    // Save the pressed key and record the time.
    currentPinState.keyPresses[currentPinState.nextPressIndex] = key;
    currentPinState.lastKeyPressTime = time(NULL);

    printf("Index %d of PIN entered. Character %c.\n", currentPinState.nextPressIndex, key);

    currentPinState.nextPressIndex++;

    // If the next key press index would be at the pin length, 
    // we just received the last key for the PIN (by length).
    // Check the pin for validity and clear the saved pin.
    if (currentPinState.nextPressIndex >= config.MAX_PIN_LENGTH)
    {
        if (checkPin(currentPinState.keyPresses))
        {
            // TODO: Do database things.
            printf("\nCORRECT PIN! - %s \n", currentPinState.keyPresses);
        }

        else
        {
            printf("\nPIN REJECTED! - %s \n", currentPinState.keyPresses);
        }

        printf("\nPress enter to continue.\n");
        getchar();

        clearKeys();
    }
}

void clearKeys()
{
    printf("PIN cleared. You may now enter another PIN.\n");

    currentPinState.lastKeyPressTime = EMPTY_TIMESTAMP;
    currentPinState.nextPressIndex = 0;

    for (int pinIndex = 0; pinIndex < config.MAX_PIN_LENGTH; pinIndex++)
    {
        currentPinState.keyPresses[pinIndex] = EMPTY_KEY;
    }
}

bool checkPin(char *pin_input)
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

bool keypressTimeOut()
{
    time_t current_time = time(NULL);
    double time_since_last_press = difftime(currentPinState.lastKeyPressTime, current_time);

    if (time_since_last_press >= config.KEYPRESS_TIMEOUT)
    {
        return true;
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



void initializeKeypad()
{
    currentPinState.nextPressIndex = 0;
    currentPinState.lastKeyPressTime = EMPTY_TIMESTAMP;
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
}

void freeKeypad()
{
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
}