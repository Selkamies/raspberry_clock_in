
#include <stdio.h>
#include <pigpio.h>

#include "gpio_handler.h"
#include "pins.h"



void mainLoop(struct GPIOPins *gpio_pins)
{
    while (!signal_received) 
    {
        // TODO: Check for any input.
        // TODO: After first input, keep track of how long it has been since last input.

        time_sleep(0.01);
    }
}



int main()
{
    printf("\nProgram starting.\n");

    struct GPIOPins gpio_pins = createGPIOPins();

    printf("\nInitializing pigpio.\n");

    // Initialized pigpio connection to the GPIO pins and starts the main loop.
    initializeGPIO(&gpio_pins, mainLoop);

    return 0;
}



