
#include <stdio.h>
#include <pigpio.h>         // time_sleep()

#include "gpio_handler.h"   // GPIO initialization.
#include "pins.h"           // Mapping GPIO pin numbers.
#include "keypad.h"         // Input handling.



void mainLoop()
{
    struct GPIOPins gpio_pins = createGPIOPins();

    printf("\nMain loop starting.\n");

    while (!signal_received) 
    {
        checkKeyPress(&gpio_pins);

        time_sleep(1);
    }

    cleanupGPIOPins(&gpio_pins);
}



int main()
{
    printf("\nProgram starting.\n");

    printf("\nInitializing pigpio.\n");

    // Initialized pigpio connection to the GPIO pins and starts the main loop.
    initializeGPIO(mainLoop);

    return 0;
}

