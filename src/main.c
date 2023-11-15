/**
 * @file main.c
 * @author Selkamies
 * 
 * @brief Clock-in application for Raspberry Pi 4. 
 * User enters PIN, and relevant user is then marked as present or not, depending if
 * they were already marked as present or not. Users and logs are stored in a database.
 * 
 * @date Created  2023-11-13
 * @date Modified 2023-11-15
 * 
 * @copyright Copyright (c) 2023
 * 
 * Possible future implementations:
 * TODO: Support for RFID tags.
 * TODO: Support for a touch screen.
 */



#include <stdio.h>
#include <pigpio.h>             // time_sleep()

#include "config_handler.h"     // Load config from config.ini.
#include "gpio_handler.h"       // GPIO initialization.
#include "pins.h"               // Mapping GPIO pin numbers.
#include "keypad.h"             // Input handling.



void mainLoop()
{
    struct GPIOPins gpio_pins = createGPIOPins();

    printf("\nMain loop starting. You may now input PIN.\n");

    // signal_received is part of pigpio, which handles access to the GPIO pins of the Raspberry Pi.
    while (!signal_received) 
    {
        //printKeyStatus();
        checkKeyPress(&gpio_pins);

        //printGPIOPinStatus(&gpio_pins);

        time_sleep(0.01);
    }

    cleanupGPIOPins(&gpio_pins);
}



int main()
{
    printf("\nProgram starting.\n");

    printf("\nLoading config.ini.\n");
    readConfigFile();

    //printf("\nInitializing keypad.\n");

    printf("\nInitializing pigpio.\n");

    // Initialized pigpio connection to the GPIO pins and starts the main loop.
    initializeGPIO(mainLoop);

    return 0;
}

