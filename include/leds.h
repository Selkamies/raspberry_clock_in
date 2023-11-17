/**
 * @file leds.c
 * @author Selkamies
 * 
 * @brief Handles the RGB led attached to the Raspberry Pi 4.
 * 
 * @date Created 2023-11-16
 * @date Modified 2023-11-16
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



void updateLED();

void turnLedOn(bool red, bool green, bool blue);

void turnLedsOff();



void setLedVariables(struct LedGPIOPins *pins, int ledStaysOnFor);

void initializeLeds();



#endif // LEDS_H