/**
 * @file pins.c
 * @author Selkamies
 * 
 * @brief Handles the mapping of GPIO pins to pin numbers, and has functions for 
 * initializing and resetting their status.
 * 
 * @date Created 2023-11-13
 * @date Modified 2023-11-17
 * 
 * @copyright Copyright (c) 2023
 * 
 * TODO: Read the pin numbers from file so that we don't have to recompile if we change pins.
 * TODO: Separate pin data and pigpio calls to their own files?
 */



#include <stdio.h>
#include <stdlib.h>     // calloc()
#include <pigpio.h>

#include "pins.h"
#include "config.h"



// Function to create and initialize the GPIO pins struct
struct GPIOPins createGPIOPins() 
{
    struct GPIOPins gpio_pins = readGPIOPinsFromFile("Dummy file name");
    initializeGPIOPins(&gpio_pins);

    return gpio_pins;
}





// TODO: Actually read the pin numbers from file.
struct GPIOPins readGPIOPinsFromFile(const char *filename)
{
    struct GPIOPins dummy_gpio_pins;

    dummy_gpio_pins.keypad_rows = calloc(config.KEYPAD_ROWS, sizeof(int));
    dummy_gpio_pins.keypad_columns = calloc(config.KEYPAD_COLUMNS, sizeof(int));

    if (dummy_gpio_pins.keypad_rows == NULL || dummy_gpio_pins.keypad_columns == NULL) 
    {
        printf("\nERROR: Memory allocation failure in pins.c, readGPIOPinsFromFile()!\n");
    }

    dummy_gpio_pins.keypad_rows[0] = 11;
    dummy_gpio_pins.keypad_rows[1] = 9;
    dummy_gpio_pins.keypad_rows[2] = 10;
    dummy_gpio_pins.keypad_rows[3] = 22;
    dummy_gpio_pins.keypad_columns[0] = 17;
    dummy_gpio_pins.keypad_columns[1] = 4;
    dummy_gpio_pins.keypad_columns[2] = 3;
    dummy_gpio_pins.keypad_columns[3] = 2;
    
    // TODO: RGP led pins.
    // TODO: Buzzer pin?
    

    return dummy_gpio_pins;
}



void initializeGPIOPins(struct GPIOPins *gpio_pins)
{
    printf("Initializing GPIO pins.\n");

    for (int index = 0; index < 4; index++)
    {
        gpioSetMode(gpio_pins->keypad_rows[index], PI_OUTPUT);
        gpioSetMode(gpio_pins->keypad_columns[index], PI_INPUT);
        gpioSetPullUpDown(gpio_pins->keypad_rows[index], PI_PUD_DOWN);
        gpioSetPullUpDown(gpio_pins->keypad_columns[index], PI_PUD_UP);
    }
}

void cleanupGPIOPins(struct GPIOPins *gpio_pins) 
{
    printf("Resetting GPIO pins.\n");

    for (int index = 0; index < 4; index++)
    {
        gpioSetMode(gpio_pins->keypad_rows[index], PI_INPUT);
        gpioSetMode(gpio_pins->keypad_columns[index], PI_INPUT);
    }

    free(gpio_pins->keypad_rows);
    free(gpio_pins->keypad_columns);
    gpio_pins->keypad_rows = NULL;
    gpio_pins->keypad_columns = NULL;
}



void printGPIOPinStatus(struct GPIOPins *gpio_pins)
{
    printf("\nGPIO pin status:\n");
    printf("%-15s%-5s%-10s%-5s%-5s\n", "Row/Column", "Idx", "Pin Number", "Mode", "Value");

    for (int row = 0; row < 4; row++)
    {
        int pin_number = gpio_pins->keypad_rows[row];
        printf("%-15s%-5d%-10d%-5d%-5d\n", "Keypad row", row, pin_number, gpioGetMode(pin_number), gpioRead(pin_number));
    }

    for (int column = 0; column < 4; column++)
    {
        int pin_number = gpio_pins->keypad_columns[column];
        printf("%-15s%-5d%-10d%-5d%-5d\n", "Keypad column", column, pin_number, gpioGetMode(pin_number), gpioRead(pin_number));
    }
}


