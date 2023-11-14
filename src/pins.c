
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

    return dummy_gpio_pins;
}



void initializeGPIOPins(struct GPIOPins *gpio_pins)
{
    gpioSetMode(gpio_pins->keypad_rows[0], PI_OUTPUT);
    gpioSetMode(gpio_pins->keypad_rows[1], PI_OUTPUT);
    gpioSetMode(gpio_pins->keypad_rows[2], PI_OUTPUT);
    gpioSetMode(gpio_pins->keypad_rows[3], PI_OUTPUT);

    gpioSetMode(gpio_pins->keypad_columns[0], PI_INPUT);
    gpioSetMode(gpio_pins->keypad_columns[1], PI_INPUT);
    gpioSetMode(gpio_pins->keypad_columns[2], PI_INPUT);
    gpioSetMode(gpio_pins->keypad_columns[3], PI_INPUT);
}

void cleanupGPIOPins(struct GPIOPins *gpio_pins) 
{
    printf("Resetting GPIO pins.\n");

    gpioSetMode(gpio_pins->keypad_rows[0], PI_INPUT);
    gpioSetMode(gpio_pins->keypad_rows[1], PI_INPUT);
    gpioSetMode(gpio_pins->keypad_rows[2], PI_INPUT);
    gpioSetMode(gpio_pins->keypad_rows[3], PI_INPUT);

    gpioSetMode(gpio_pins->keypad_columns[0], PI_INPUT);
    gpioSetMode(gpio_pins->keypad_columns[1], PI_INPUT);
    gpioSetMode(gpio_pins->keypad_columns[2], PI_INPUT);
    gpioSetMode(gpio_pins->keypad_columns[3], PI_INPUT);
}



void printGPIOPinNumbers(struct GPIOPins *gpio_pins)
{
    printf("GPIO pin numbers:\n");
    printf("Keypad row 1: %d\n", gpio_pins->keypad_rows[0]);
    printf("Keypad row 2: %d\n", gpio_pins->keypad_rows[1]);
    printf("Keypad row 3: %d\n", gpio_pins->keypad_rows[2]);
    printf("Keypad row 4: %d\n", gpio_pins->keypad_rows[3]);

    printf("Keypad column 1: %d\n", gpio_pins->keypad_columns[0]);
    printf("Keypad column 2: %d\n", gpio_pins->keypad_columns[1]);
    printf("Keypad column 3: %d\n", gpio_pins->keypad_columns[2]);
    printf("Keypad column 4: %d\n", gpio_pins->keypad_columns[3]);
}


