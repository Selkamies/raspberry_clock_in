/**
 * @file main.c
 * @author Selkamies
 * 
 * @brief Clock-in application for Raspberry Pi 4. 
 * User enters PIN, and relevant user is then marked as present or not, depending if
 * they were already marked as present or not. Users and logs are stored in a database.
 * 
 * @date Created  2023-11-13
 * @date Modified 2023-11-16
 * 
 * @copyright Copyright (c) 2023
 * 
 * Possible future implementations:
 * TODO: Support for RFID tags.
 * TODO: Support for a touch screen.
 */



#include <stdio.h>              // printf().

#include "config_handler.h"     // Load config from config.ini.
#include "gpio_handler.h"       // Pigpio initialization.
// TODO: Stop using this here, use in keypad and leds.
#include "pins.h"               // Mapping GPIO pin numbers.
#include "keypad.h"             // Input handling.
#include "leds.h"               // We update led status here.



void mainLoop()
{
    // TODO: Don't create this here, create pin struct for keypad in keypad and for leds in leds.
    struct GPIOPins gpio_pins = createGPIOPins();

    printf("\nMain loop starting. You may now input PIN.\n");

    // signal_received is part of pigpio, which handles access to the GPIO pins of the Raspberry Pi.
    while (!signal_received) 
    {
        updateKeypad(&gpio_pins);
        updateLED();

        pigpioSleep(0.1);
    }

    // TODO: Move this to cleanup(). Stop passing gpio_pins around.
    cleanupGPIOPins(&gpio_pins);
}



/**
 * @brief Read files, set up variables, start pigpio.
 */
void initialize()
{
    initializePigpio();

    readConfigFile();
    initializeKeypad();
    
    mainLoop();
}

/**
 * @brief Freeing memory, turning off leds.
 */
void cleanup()
{
    cleanupKeypad();

    cleanupPigpio();
}



int main()
{
    printf("\nProgram starting.\n");

    initialize();
    cleanup();

    return 0;
}

