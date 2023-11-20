/**
 * @file keypad_gpio.c
 * @author Selkamies
 * 
 * @brief Handles all the GPIO pin operations required by keypad using pigpio.
 * 
 * @date Created 2023-11-13
 * @date Updated 2023-11-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */



#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pigpio.h>

#include "keypad_gpio.h"
#include "keypad.h"



void turnKeypadRowOn(int keypadRowPin)
{
    gpioWrite(keypadRowPin, 1);
}

void turnKeypadRowOff(int keypadRowPin)
{
    gpioWrite(keypadRowPin, 0);
}

bool isKeypadColumnOff(int keypadColumnPin)
{
    if (gpioRead(keypadColumnPin))
    {
        return false;
    }

    else
    {
        return true;
    }
}

void initializeGPIOPins(struct KeypadGPIOPins *keypadPins)
{
    printf("Initializing GPIO pins.\n");

    for (int index = 0; index < 4; index++)
    {
        gpioSetMode(keypadPins->keypad_rows[index], PI_OUTPUT);
        gpioSetMode(keypadPins->keypad_columns[index], PI_INPUT);
        gpioSetPullUpDown(keypadPins->keypad_rows[index], PI_PUD_DOWN);
        gpioSetPullUpDown(keypadPins->keypad_columns[index], PI_PUD_UP);
    }
}

void cleanupGPIOPins(struct KeypadGPIOPins *keypadPins) 
{
    printf("Resetting GPIO pins.\n");

    for (int index = 0; index < 4; index++)
    {
        gpioSetMode(keypadPins->keypad_rows[index], PI_INPUT);
        gpioSetMode(keypadPins->keypad_columns[index], PI_INPUT);
    }

    free(keypadPins->keypad_rows);
    free(keypadPins->keypad_columns);
    keypadPins->keypad_rows = NULL;
    keypadPins->keypad_columns = NULL;
}