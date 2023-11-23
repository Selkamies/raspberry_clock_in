/**
 * @file keypad_functions.c
 * @author Selkamies
 * 
 * @brief Handles all the GPIO pin operations required by keypad using pigpio.
 * 
 * @date Created 2023-11-13
 * @date Updated 2023-11-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */



#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pigpio.h>

#include "gpio_functions.h"
#include "keypad.h"



void turnGPIOPinOn(int pinNumber)
{
    gpioWrite(pinNumber, 1);
}

void turnGPIOPinOff(int pinNumber)
{
    gpioWrite(pinNumber, 0);
}

bool isGPIOPinOn(int pinNumber)
{
    if (gpioRead(pinNumber))
    {
        return false;
    }

    else
    {
        return true;
    }
}

void initializeKeypadGPIOPins(struct KeypadGPIOPins *keypadPins, struct KeypadConfig *config)
{
    printf("Initializing keypad GPIO pins.\n");

    // Keypad rows are set to output and pulldown state.
    for (int rowIndex = 0; rowIndex < config->KEYPAD_ROWS; rowIndex++)
    {
        gpioSetMode(keypadPins->keypad_rows[rowIndex], PI_OUTPUT);
        gpioSetPullUpDown(keypadPins->keypad_rows[rowIndex], PI_PUD_DOWN);
    }

    // Keypad rows are set to input and pullup state.
    for (int columnIndex = 0; columnIndex < config->KEYPAD_COLUMNS; columnIndex++)
    {
        gpioSetMode(keypadPins->keypad_columns[columnIndex], PI_INPUT);
        gpioSetPullUpDown(keypadPins->keypad_columns[columnIndex], PI_PUD_UP);
    }
}

void cleanupKeypadGPIOPins(struct KeypadGPIOPins *keypadPins, struct KeypadConfig *config) 
{
    printf("Resetting GPIO pins.\n");

    for (int rowIndex = 0; rowIndex < config->KEYPAD_ROWS; rowIndex++)
    {
        gpioSetMode(keypadPins->keypad_rows[rowIndex], PI_INPUT);
    }

    for (int columnIndex = 0; columnIndex < config->KEYPAD_COLUMNS; columnIndex++)
    {
        gpioSetMode(keypadPins->keypad_columns[columnIndex], PI_INPUT);
    }

    free(keypadPins->keypad_rows);
    free(keypadPins->keypad_columns);
    keypadPins->keypad_rows = NULL;
    keypadPins->keypad_columns = NULL;
}

