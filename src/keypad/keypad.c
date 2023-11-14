
//#include <pigpio.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "pins.h"
#include "keypad.h"
#include "keypad_gpio.h"



/* Current state of the pin the user is trying to input, if any. */
struct CurrentPinInput currentPinState =
{
    .nextPressIndex = 0,
    .lastKeyPressTime = EMPTY_TIMESTAMP,
    .keyPresses = {EMPTY_KEY}  
};

/* Keys on the keypad and their state when previously checked. */
struct KeyPad keyPadState =
{
    .keys =
    {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    },
    .keysPressedPreviously = {{false}}
};



char checkKeyPress(struct GPIOPins *gpioPins)
{
    char pressedKey = EMPTY_KEY;

    for (int row = 0; row < KEYPAD_ROWS; row++)
    {
        // Disable the current row to check if any key in this row is pressed.
        turnKeypadRowOff(gpioPins->keypad_rows[row]);

        // Check every column pin to see if a key in this row is pressed.
        for (int column = 0; column < KEYPAD_COLUMNS; column++)
        {
            bool keyNowPressed = isKeypadColumnOff(gpioPins->keypad_columns[column]);

            if (keyNowPressed && !keyPadState.keysPressedPreviously[row][column])
            {
                pressedKey = keyPadState.keys[row][column];

                keyPadState.keysPressedPreviously[row][column] = true;
                printf("Key %c was just pressed.\n", pressedKey);
            }

            else if (!keyNowPressed)
            {
                keyPadState.keysPressedPreviously[row][column] = false;
            }
        }

        // Enable the current row to check the next one.
        turnKeypadRowOn(gpioPins->keypad_rows[row]);
    }

    return pressedKey;
}

void storeKeyPress(char key)
{
    // Save the pressed key and record the time.
    currentPinState.keyPresses[currentPinState.nextPressIndex] = key;
    currentPinState.lastKeyPressTime = time(NULL);

    printf("Index %d of pin code entered. Character %c.\n", currentPinState.nextPressIndex, key);

    currentPinState.nextPressIndex++;

    // If the next key press index would be at the pin length, 
    // we just received the last key for the pin code (by length).
    // Check the pin for validity and clear the saved pin.
    if (currentPinState.nextPressIndex >= MAX_PIN_LENGTH)
    {
        if (checkPin())
        {
            // TODO: Grant access.
        }

        clearKeys();
    }
}

void clearKeys()
{
    currentPinState.lastKeyPressTime = EMPTY_TIMESTAMP;
    currentPinState.nextPressIndex = 0;

    for (int pinIndex = 0; pinIndex < MAX_PIN_LENGTH; pinIndex++)
    {
        currentPinState.keyPresses[pinIndex] = EMPTY_KEY;
    }
}

bool checkPin()
{
    return false;
}

bool keypressTimeOut()
{
    time_t current_time = time(NULL);
    double time_since_last_press = difftime(currentPinState.lastKeyPressTime, current_time);

    if (time_since_last_press >= KEYPRESS_TIMEOUT)
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

    for (int row = 0; row < KEYPAD_ROWS; row++)
    {
        printf("%d %d %d %d\n", keyPadState.keysPressedPreviously[row][0], keyPadState.keysPressedPreviously[row][1], 
                                keyPadState.keysPressedPreviously[row][2], keyPadState.keysPressedPreviously[row][3]);
    }
}