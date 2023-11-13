
#include <stdio.h>
#include <signal.h>     // sig_atomic_t
#include <pigpio.h>

#include "gpio_handler.h"
#include "pins.h"



volatile sig_atomic_t signal_received = 0;

void signalHandler(int signo) 
{
    if (signo == SIGINT) 
    {
        signal_received = 1;
    }
}

void initializeGPIO(struct GPIOPins *gpio_pins, void (*mainLoop)(struct GPIOPins *)) 
{
    if (gpioInitialise() < 0) 
    {
        fprintf(stderr, "Failed to initialize pigpio\n");
        // Handle initialization failure
    }

    // Now that pigpio is working, we can set the initial states of the pins.
    initializeGPIOPins(gpio_pins);

    // Set up signal handler
    signal(SIGINT, signalHandler);

    printf("\nMain loop starting.\n");

    // Start the main loop.
    mainLoop(gpio_pins);

    cleanupGPIOPins(gpio_pins);
    gpioTerminate();
}