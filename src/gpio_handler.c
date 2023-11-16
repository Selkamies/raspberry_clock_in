
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

void initializePigpio() 
{
    if (gpioInitialise() < 0) 
    {
        fprintf(stderr, "Failed to initialize pigpio\n");
        // Handle initialization failure
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