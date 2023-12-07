/**
 * @file keypad_functions.c
 * @author Selkamies
 * 
 * @brief Handles all the GPIO pin operations required by keypad using pigpio.
 * 
 * @date Created 2023-11-13
 * @date Updated 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */



#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pigpio.h>

#include "gpio_functions.h"
#include "keypad_config.h"



void turnGPIOPinOn(const int pinNumber)
{
    gpioWrite(pinNumber, 1);
}

void turnGPIOPinOff(const int pinNumber)
{
    gpioWrite(pinNumber, 0);
}

bool isGPIOPinOn(const int pinNumber)
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

void initializeKeypadGPIOPins(struct KeypadConfig *keypadConfig)
{
    printf("Initializing keypad GPIO pins.\n");

    // Keypad rows are set to output and pulldown state.
    for (int rowIndex = 0; rowIndex < keypadConfig->KEYPAD_ROWS; rowIndex++)
    {
        gpioSetMode(keypadConfig->pins.keypad_rows[rowIndex], PI_OUTPUT);
        gpioSetPullUpDown(keypadConfig->pins.keypad_rows[rowIndex], PI_PUD_DOWN);
    }

    // Keypad rows are set to input and pullup state.
    for (int columnIndex = 0; columnIndex < keypadConfig->KEYPAD_COLUMNS; columnIndex++)
    {
        gpioSetMode(keypadConfig->pins.keypad_columns[columnIndex], PI_INPUT);
        gpioSetPullUpDown(keypadConfig->pins.keypad_columns[columnIndex], PI_PUD_UP);
    }
}

void cleanupKeypadGPIOPins(struct KeypadConfig *keypadConfig) 
{
    printf("Resetting GPIO pins.\n");

    for (int rowIndex = 0; rowIndex < keypadConfig->KEYPAD_ROWS; rowIndex++)
    {
        gpioSetMode(keypadConfig->pins.keypad_rows[rowIndex], PI_INPUT);
    }

    for (int columnIndex = 0; columnIndex < keypadConfig->KEYPAD_COLUMNS; columnIndex++)
    {
        gpioSetMode(keypadConfig->pins.keypad_columns[columnIndex], PI_INPUT);
    }

    free(keypadConfig->pins.keypad_rows);
    free(keypadConfig->pins.keypad_columns);
    keypadConfig->pins.keypad_rows = NULL;
    keypadConfig->pins.keypad_columns = NULL;
}

