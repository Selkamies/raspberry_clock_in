
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

void initializeGPIO(void (*mainLoop)()) 
{
    if (gpioInitialise() < 0) 
    {
        fprintf(stderr, "Failed to initialize pigpio\n");
        // Handle initialization failure
    }

    // Set up signal handler
    signal(SIGINT, signalHandler);

    // Start the main loop.
    mainLoop();

    gpioTerminate();
}