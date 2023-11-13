
#include <stdio.h>
#include <pigpio.h>

#include "pins.h"



// Function to create and initialize the GPIO pins struct
struct GPIOPins createGPIOPins() 
{
    struct GPIOPins gpio_pins = readGPIOPinsFromFile("Dummy file name");
    //initializeGPIOPins(&gpio_pins);

    return gpio_pins;
}



// TODO: Actually read the pin numbers from file.
struct GPIOPins readGPIOPinsFromFile(const char *filename)
{
    struct GPIOPins dummy_gpio_pins = 
    {
        .keypad_row_1 = 11,
        .keypad_row_2 = 9,
        .keypad_row_3 = 10,
        .keypad_row_4 = 22,
        .keypad_column_1 = 17,
        .keypad_column_2 = 4,
        .keypad_column_3 = 3,
        .keypad_column_4 = 2
        // TODO: RGP led pin.
        // TODO: Buzzer pin?
    };

    return dummy_gpio_pins;
}



void initializeGPIOPins(struct GPIOPins *gpio_pins)
{
    gpioSetMode(gpio_pins->keypad_row_1, PI_OUTPUT);
    gpioSetMode(gpio_pins->keypad_row_2, PI_OUTPUT);
    gpioSetMode(gpio_pins->keypad_row_3, PI_OUTPUT);
    gpioSetMode(gpio_pins->keypad_row_4, PI_OUTPUT);

    gpioSetMode(gpio_pins->keypad_column_1, PI_INPUT);
    gpioSetMode(gpio_pins->keypad_column_2, PI_INPUT);
    gpioSetMode(gpio_pins->keypad_column_3, PI_INPUT);
    gpioSetMode(gpio_pins->keypad_column_4, PI_INPUT);
}

void cleanupGPIOPins(struct GPIOPins *gpio_pins) 
{
    gpioSetMode(gpio_pins->keypad_row_1, PI_INPUT);
    gpioSetMode(gpio_pins->keypad_row_2, PI_INPUT);
    gpioSetMode(gpio_pins->keypad_row_3, PI_INPUT);
    gpioSetMode(gpio_pins->keypad_row_4, PI_INPUT);

    gpioSetMode(gpio_pins->keypad_column_1, PI_INPUT);
    gpioSetMode(gpio_pins->keypad_column_2, PI_INPUT);
    gpioSetMode(gpio_pins->keypad_column_3, PI_INPUT);
    gpioSetMode(gpio_pins->keypad_column_4, PI_INPUT);
}



void printGPIOPinNumbers(struct GPIOPins *gpio_pins)
{
    printf("GPIO pin numbers:\n");
    printf("Keypad row 1: %d\n", gpio_pins->keypad_row_1);
    printf("Keypad row 2: %d\n", gpio_pins->keypad_row_2);
    printf("Keypad row 3: %d\n", gpio_pins->keypad_row_3);
    printf("Keypad row 4: %d\n", gpio_pins->keypad_row_4);

    printf("Keypad column 1: %d\n", gpio_pins->keypad_column_1);
    printf("Keypad column 2: %d\n", gpio_pins->keypad_column_2);
    printf("Keypad column 3: %d\n", gpio_pins->keypad_column_3);
    printf("Keypad column 4: %d\n", gpio_pins->keypad_column_4);
}


