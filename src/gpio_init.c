/**
 * @file gpio_init.c
 * @author Selkamies
 * 
 * @brief Manages the pigpio library initialization. pigpio handles the GPIO pins of Raspberry Pi.
 * 
 * @date Created 2023-11-13
 * @date Modified 2023-11-23
 * 
 * @copyright Copyright (c) 2023
 */



#include <stdio.h>      // printf().
#include <pigpio.h>     // gpioInitialize(), time_sleep(), gpioTerminate().
#include <stdbool.h>

#include "gpio_init.h"



volatile sig_atomic_t signal_received = 0;

void signalHandler(int signo) 
{
    if (signo == SIGINT) 
    {
        signal_received = 1;
    }
}

bool initializeGPIOLibrary() 
{
    printf("Initializing pigpio.\n");

    if (gpioInitialise() < 0) 
    {
        fprintf(stderr, "Failed to initialize pigpio\n");
        // TODO: Exit program. 
        return false;
    }

    // Set up signal handler
    signal(SIGINT, signalHandler);

    return true;
}

void sleepGPIOLibrary(double seconds)
{
    time_sleep(seconds);
}

void cleanupGPIOLibrary()
{
    gpioTerminate();
}