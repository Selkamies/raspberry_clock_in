
//#include <pigpio.h>
#include <stdbool.h>
#include <time.h>

//#include "pins.h"
#include "keypad.h"
#include "keypad_gpio.h"



struct CurrentPinCode currentPinState =
{
    .nextPressIndex = 0,
    .lastKeyPressTime = EMPTY_TIMESTAMP,
    .keyPresses = {EMPTY_KEY}  
};

struct KeyPad keyPadState
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



struct GPIOPins
{
    int keypad_rows[KEYPAD_ROWS];
    int keypad_columns[KEYPAD_COLUMNS];
};

struct GPIOPins gpio_pins = 
{
    .keypad_rows[0] = 11,
    .keypad_rows[1] = 9,
    .keypad_rows[2] = 10,
    .keypad_rows[3] = 22,
    .keypad_columns[0] = 17,
    .keypad_columns[1] = 4,
    .keypad_columns[2] = 3,
    .keypad_columns[3] = 2
    // TODO: RGP led pin.
    // TODO: Buzzer pin?
};





void checkKeyPress(struct GPIOPins *gpioPins)
{
    char pressedKey = EMPTY_KEY;

    for (int row = 0; row < KEYPAD_ROWS; row++)
    {
        // Enable the current row to check if any key in this row is pressed.
        turnKeypadRowOn(row);

        // Check every column pin to see if a key in this row is pressed.
        for (int column = 0; column < KEYPAD_COLUMNS; column++)
        {
            bool keyNowPressed = isKeypadColumnOn(column);

            if (keyNowPressed == 1 && !keyPadState.keysPressedPreviously[row][column])
            {
                pressedKey = keys[row][column];

                keyPadState.keysPressedPreviously[row][column] = true;
                printf("Key %c was just pressed.\n", pressedKey)
            }

            else if (!keyNowPressed)
            {
                keyPadState.keysPressedPreviously[row][column] = false;
            }
        }

        // Disable the current row to check the next one.
        turnKeypadRowOn(row);
    }

    return pressedKey;
}

void storeKeyPress(char key)
{
    // Save the pressed key and record the time.
    currentPinState.keyPresses[currentPinState.nextPressIndex].key = key;
    currentPinState.lastKeyPressTime = time(NULL);

    currentPinState.nextPressIndex++;

    // If the next key press index would be at the pin length, 
    // we just received the last key for the pin code (by length).
    // Check the pin for validity and clear the saved pin.
    if (currentPinState.nextPressIndex => MAX_PIN_LENGTH)
    {
        if checkPin()
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
        currentPinState.keyPresses[pinIndex].key = EMPTY_KEY;
    }
}

bool checkPin()
{
    return false;
}