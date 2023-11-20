/**
 * @file leds.c
 * @author Selkamies
 * 
 * @brief Handles the RGB led attached to the Raspberry Pi 4.
 * 
 * @date Created 2023-11-16
 * @date Modified 2023-11-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */



#ifndef LEDS_H
#define LEDS_H



#include <time.h>           // time_t.
#include <stdbool.h>



// TODO: Use the struct with all the pins in pins.h?
struct LedGPIOPins 
{
    int LED_RED;
    int LED_GREEN;
    int LED_BLUE;
};

struct LedStatus
{
    // Whether any led is on.
    bool ledIsOn;
    // When led was turned on.
    time_t ledStartTime;
    // How many seconds the led stays on for.
    int ledStaysOnFor;
};



/**
 * @brief Update led status. If led is on, check if enough time has passed to turn them off.
 */
void updateLED();

/**
 * @brief Turns the RGB led on, by setting the three primary colors on or off.
 * 
 * @param red Whether to use red light or not.
 * @param green Whether to use green light or not.
 * @param blue Whether to use blue light or not.
 */
void turnLedOn(bool red, bool green, bool blue);

/**
 * @brief Turns the RGB led off.
 */
void turnLedsOff();



/**
 * @brief Set the variables used by leds that need to be read from file. Called by config_handler.
 * 
 * @param pins Struct with GPIO pin numbers for the RGB led.
 * @param ledStaysOnFor Time in seconds that the RGB led stays on for when turned on.
 */
void setLedVariables(struct LedGPIOPins *pins, int ledStaysOnFor);

/**
 * @brief Initialize some variables to their default states.
 */
void initializeLeds();



#endif // LEDS_H