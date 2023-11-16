/**
 * @file gpio_handler.c
 * @author Selkamies
 * 
 * @brief Manages the pigpio library initialization. pigpio handles the GPIO pins of Raspberry Pi.
 * 
 * @date Created 2023-11-13
 * @date Modified 2023-11-16
 * 
 * @copyright Copyright (c) 2023
 */



#include <stdio.h>      // printf().
#include <pigpio.h>     // gpioInitialize(), time_sleep(), gpioTerminate().

#include "gpio_handler.h"



volatile sig_atomic_t signal_received = 0;

void signalHandler(int signo) 
{
    if (signo == SIGINT) 
    {
        signal_received = 1;
    }
}

void initializePigpio() 
{
    printf("Initializing pigpio.\n");

    if (gpioInitialise() < 0) 
    {
        fprintf(stderr, "Failed to initialize pigpio\n");
        // TODO: Exit program. 
    }

    // Set up signal handler
    signal(SIGINT, signalHandler);
}

void pigpioSleep(double seconds)
{
    time_sleep(seconds);
}

void cleanupPigpio()
{
    gpioTerminate();
}